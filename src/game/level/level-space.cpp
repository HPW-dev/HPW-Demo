#include <cassert>
#include "level-space.hpp"
#include "game/util/sync.hpp"
#include "game/util/game-util.hpp"
#include "game/util/post-effect/post-effects.hpp"
#include "game/util/post-effect/blink-text.hpp"
#include "game/util/post-effect/game-post-effects.hpp"
#include "game/core/common.hpp"
#include "game/core/core.hpp"
#include "game/core/canvas.hpp"
#include "game/core/graphic.hpp"
#include "game/core/entities.hpp"
#include "game/core/levels.hpp"
#include "game/level/util/level-tasks.hpp"
#include "game/level/util/level-util.hpp"
#include "game/level/util/tilemap.hpp"
#include "game/level/util/layer-simple.hpp"
#include "game/level/util/task-spawn.hpp"
#include "game/entity/player/player.hpp"
#include "game/entity/util/entity-util.hpp"
#include "util/hpw-util.hpp"
#include "util/math/random.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/effect/dither.hpp"

struct Level_space::Impl {
  constx real layer_speed_scale {0.03333}; // влияет на скорость движния фона по горизонтали
  constx real layer_h_speed {1.25_pps}; // скорость движния фона по вертикали
  
  Level_tasks tasks {};
  bool fill_bg_black {true}; // закрашивать фон чёрным
  bool enable_bg_layer {false}; // включает отрисовку фона с космосом
  bool enable_galaxy_1_layer {false}; // включает отрисовку фона с первой галактикой
  bool enable_galaxy_2_layer {false}; // включает отрисовку фона с большой галактикой
  bool enable_galaxy_3_layer {false}; // включает отрисовку фона со второй галактикой
  mutable real bg_brightness_sub {1.0}; // затенение фона
  mutable real galaxy_2_brightness_sub {1.0}; // затенение фона галактики
  Layer_simple m_bg {}; // чёрный фон со звёздочками
  Layer_simple m_galaxy_1 {}; // права галактика
  Layer_simple m_galaxy_2 {}; // большая финальная галактика
  Layer_simple m_galaxy_3 {}; // первая левая галактика
  real m_waiter_accel_additon {}; // ускоряет появление новых ждунов
  bool predict_waiter {false}; // если включён, ждун будет подбирать место спавна получше

  inline Impl()
    : m_bg{"resource/image/bg/space/bg/tilemap.yml", Vec{}, 0.025, &blend_fade_out_max}
    , m_galaxy_1{"resource/image/bg/space/galaxy 1/tilemap.yml", Vec{}, 0.1, &blend_max}
    , m_galaxy_2{"resource/image/bg/space/galaxy 2/tilemap.yml", Vec{}, 0.025001, &blend_fade_out_max}
    , m_galaxy_3{"resource/image/bg/space/galaxy 3/tilemap.yml", Vec{}, 0.05, &blend_max}
  {
    hpw::shmup_mode = true;
    init_collider();
    make_player();
    init_tasks();

    // надпись о низкой гравитации
    // TODO locale
    graphic::post_effects->move_to_back (
      new_shared<Blink_text>(6, U"слабая гравитация") );
  }

  inline void update(const Vec vel, Delta_time dt) {
    auto vel_inverted = vel * -1.0;
    execute_tasks(tasks, dt);

    if (enable_bg_layer)
      update_layers(vel_inverted, dt);
  }

  inline void draw(Image& dst) const {
    // отрисовка уровня:
    if (fill_bg_black)
      dst.fill(Pal8::black);
    draw_bg_layers(dst);
  }

  inline void make_player() {
    auto player_pos_from_prev_level = hpw::level_mgr->player_prev_lvl_pos();

    cauto pos = player_pos_from_prev_level.not_zero()
      ? player_pos_from_prev_level
      : get_screen_center();
    hpw::entity_mgr->make({}, "player.boo.dark", pos);
    // на этом уорвне у игрока гравитация снижена
    auto player = hpw::entity_mgr->get_player();
    assert(player);
    player->default_force *= 0.004;
    player->focus_force *= 0.02;
  }

  inline void init_collider() {
    set_default_collider();
  }

  // наполнение уровня тригерами
  inline void init_tasks() {

    // справнит ждуна сверху экрана
    Spwan_and_wait_for_death waiter_maker(
      [this]->Spwan_and_wait_for_death::Death_list {
        auto ret = hpw::entity_mgr->make({}, "enemy.cosmic.waiter",
          Vec(rndr(30, graphic::width-30), -30));
        
        // будет ли ждун целиться в игрока или появлятся рандомно
        if (predict_waiter) {
          Vec spawn_pos = ret->phys.get_pos();
          switch (rndu(2)) {
            default:
            case 0: { // появиться где игрок
              auto player = hpw::entity_mgr->get_player();
              spawn_pos.x = player->phys.get_pos().x;
              break;
            }
            case 1: { // полететь на упреждение
              auto player = hpw::entity_mgr->get_player();
              auto predict_point = predict(*ret, *player, hpw::target_update_time);
              spawn_pos.x = predict_point.x;
              break;
            }
          } // switch set spawn_pos
          ret->phys.set_pos(spawn_pos);
        }
        predict_waiter = !predict_waiter; // чередовать способ спавна

        // ждун каждый раз появляется с ускорением
        ret->phys.set_accel(ret->phys.get_accel() + m_waiter_accel_additon);
        m_waiter_accel_additon += pps(7); // следующее появление будет ещё быстрее
        return {ret};
      },
      6 // столько ждать до спавна след-о ждуна
    ); // 

    tasks = {
      
      // при старте, фон не стирается некоторое время
      Bg_blink(fill_bg_black, 6.0),
      
      // включить показ фона из тени
      [this](Delta_time dt)->bool {
        enable_bg_layer = true;
        bg_brightness_sub = 1;
        // поставить фон в центр
        m_bg.pos = -Vec(
          m_bg.tilemap.get_original_w() / 2.0 - graphic::width / 2.0,
          m_bg.tilemap.get_original_h() - graphic::height
        );
        return true; 
      },

      // включить показ первой галактики
      [this](Delta_time dt)->bool {
        enable_galaxy_1_layer = true;
        // галактика сверху слева
        m_galaxy_1.pos = -Vec(100, 300);
        return true; 
      },

      /*Spwan_and_wait_for_death( []{
        return Spwan_and_wait_for_death::Death_list {
          hpw::entity_mgr->make({}, "enemy.illaen", get_screen_center())
        };
      }, 5),*/
      
      // ждуны
      waiter_maker,
      waiter_maker,
      waiter_maker,
      waiter_maker,
      waiter_maker,
      waiter_maker,
      waiter_maker,
      waiter_maker,
      waiter_maker,

      // охотник сверху
      Spwan_and_wait_for_death( []{
        return Spwan_and_wait_for_death::Death_list {
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(graphic::width / 2.0, -30))
        };
      }, 20),
      // два охотника сверху
      Spwan_and_wait_for_death( []{
        return Spwan_and_wait_for_death::Death_list {
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec((graphic::width / 2.0) - 100, -30)),
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec((graphic::width / 2.0) + 100, -30)),
        };
      }, 20),
      // три охотника сверху
      Spwan_and_wait_for_death( []{
        return Spwan_and_wait_for_death::Death_list {
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(0, -30)),
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(graphic::width / 2.0,  -30)),
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(graphic::width, -30)),
        };
      }, 20),
      // охотники сбоку
      Spwan_and_wait_for_death( []{
        return Spwan_and_wait_for_death::Death_list {
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(-30, graphic::height/2)),
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(graphic::width+30, graphic::height/2)),
        };
      }, 20),

      // космические глаза x1
      Spwan_and_wait_for_death( []{
        return Spwan_and_wait_for_death::Death_list {
          hpw::entity_mgr->make({}, "enemy.cosmic", Vec(
            rndr(120, graphic::width-120),
            rndr(120, graphic::height-120)
          ) )
        };
      }, 60),

      // включить показ второй галактики
      [this](Delta_time dt)->bool {
        enable_galaxy_3_layer = true;
        // галактика сверху слева
        m_galaxy_3.pos = -Vec(-250, 190);
        return true; 
      },

      // охотники сбоку снизу
      Spwan_and_wait_for_death( []{
        return Spwan_and_wait_for_death::Death_list {
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(-30, graphic::height)),
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(graphic::width+30, graphic::height)),
        };
      }, 10),
      // охотники сбоку снизу и сбоку сверху
      Spwan_and_wait_for_death( []{
        return Spwan_and_wait_for_death::Death_list {
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(-30, graphic::height)),
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(-30, 0)),
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(graphic::width+30, graphic::height)),
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(graphic::width+30, 0)),
        };
      }, 20),
      // охотники сверху в ряд
      Spwan_and_wait_for_death( []{
        Spwan_and_wait_for_death::Death_list ret;
        cauto count = 6;
        cfor (i, count) {
          auto it = hpw::entity_mgr->make({}, "enemy.cosmic.hunter",
            Vec((graphic::width / (count-1)) * i, -30));
          ret.emplace_back(it);
        }
        return ret;
      }, 30),

      // неожиданные ждунчики
      waiter_maker,
      waiter_maker,

      // TODO большая волна слабых противников

      // включить показ фоновой большой галактики
      [this](Delta_time dt)->bool {
        enable_galaxy_2_layer = true;
        galaxy_2_brightness_sub = 1;
        // галактика в центре сверху
        m_galaxy_2.pos = -Vec(
          m_galaxy_2.tilemap.get_original_w() / 2.0 - graphic::width / 2.0,
          310
        );
        return true; 
      },

      // космические глаза x2
      Spwan_and_wait_for_death( []{
        return Spwan_and_wait_for_death::Death_list {
          hpw::entity_mgr->make({}, "enemy.cosmic", Vec(
            rndr(120, graphic::width-120),
            rndr(120, graphic::height-120)
          ) ),
          hpw::entity_mgr->make({}, "enemy.cosmic", Vec(
            rndr(120, graphic::width-120),
            rndr(120, graphic::height-120)
          ) )
        };
      }, 120),
      
      // охотники сбоку
      Spwan_and_wait_for_death( []{
        return Spwan_and_wait_for_death::Death_list {
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(-30, graphic::height/2)),
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(graphic::width+30, graphic::height/2)),
        };
      }, 20),
      // много охотников сбоку
      Spwan_and_wait_for_death( []{
        return Spwan_and_wait_for_death::Death_list {
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(-30, graphic::height)),
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(-30, graphic::height/2)),
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(-30, 0)),
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(graphic::width+30, graphic::height)),
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(graphic::width+30, graphic::height/2)),
          hpw::entity_mgr->make({}, "enemy.cosmic.hunter", Vec(graphic::width+30, 0)),
        };
      }, 80),

      // космические глаза x3
      Spwan_and_wait_for_death( []{
        return Spwan_and_wait_for_death::Death_list {
          hpw::entity_mgr->make({}, "enemy.cosmic", Vec(
            rndr(120, graphic::width-120),
            rndr(120, graphic::height-120)
          ) ),
          hpw::entity_mgr->make({}, "enemy.cosmic", Vec(
            rndr(120, graphic::width-120),
            rndr(120, graphic::height-120)
          ) ),
          hpw::entity_mgr->make({}, "enemy.cosmic", Vec(
            rndr(120, graphic::width-120),
            rndr(120, graphic::height-120)
          ) )
        };
      }, 120),

      // TODO табличка что уровень закончился
      [this](Delta_time dt)->bool {
        graphic::post_effects->move_to_back (
          new_shared<Blink_text>(10, U"дальше уровень недоделан, ничего не будет") );
        return true;
      },
      
    }; // tasks
  } // init_tasks

  inline void draw_bg_layers(Image& dst) const {
    draw_bg_layer(dst);
    draw_galaxy_1_layer(dst);
    draw_galaxy_2_layer(dst);
    draw_galaxy_3_layer(dst);
    fast_dither_bayer16x16_4bit(dst, true);
  }

  inline void draw_bg_layer(Image& dst) const {
    return_if (!enable_bg_layer);
    // фон постепенно появляется из тени
    if (bg_brightness_sub > 0)
      m_bg.draw(dst, bg_brightness_sub * 255.0);
    else
      m_bg.draw(dst);
  }

  inline void draw_galaxy_1_layer(Image& dst) const {
    return_if (!enable_galaxy_1_layer);
    m_galaxy_1.draw(dst);
  }

  inline void draw_galaxy_2_layer(Image& dst) const {
    return_if (!enable_galaxy_2_layer);
    // если фон выходит из тени, то рисовать его на отдельную картинку и её вставлять
    if (galaxy_2_brightness_sub > 0) {
      m_galaxy_2.draw(dst, galaxy_2_brightness_sub * 255.0);
    } else { // до конца осветлённый фон рисовать как обычно
      m_galaxy_2.draw(dst);
    }
  } // draw_galaxy_2_layer

  inline void draw_galaxy_3_layer(Image& dst) const {
    return_if (!enable_galaxy_3_layer);
    m_galaxy_3.draw(dst);
  }

  inline void update_layers(const Vec vel, Delta_time dt) {
    update_bg_layer(vel, dt);
    update_galaxy_1_layer(vel, dt);
    update_galaxy_2_layer(vel, dt);
    update_galaxy_3_layer(vel, dt);
  }

  inline void update_bg_layer(const Vec vel, Delta_time dt) {
    // фон постепенно появляется из тени
    if (enable_bg_layer) {
      if (bg_brightness_sub > 0)
        bg_brightness_sub -= dt * 0.11717; // скорость появления фона
      else
        m_bg.bf = &blend_past;
    }

    m_bg.update(Vec(vel.x * layer_speed_scale, layer_h_speed), dt);
    
    // следить, чтобы не выйти за пределы экрана
    auto w = m_bg.tilemap.get_original_w() - graphic::width;
    auto h = m_bg.tilemap.get_original_h() - graphic::height;
    assert(w);
    assert(h);
    m_bg.pos.x = std::clamp<real>(m_bg.pos.x, -w, 0);
    m_bg.pos.y = std::clamp<real>(m_bg.pos.y, -h, 0);
  }

  inline void update_galaxy_1_layer(const Vec vel, Delta_time dt) {
    return_if ( !enable_galaxy_1_layer);
    m_galaxy_1.update(Vec(vel.x * layer_speed_scale, layer_h_speed), dt);
  }

  inline void update_galaxy_2_layer(const Vec vel, Delta_time dt) {
    if (enable_galaxy_2_layer && galaxy_2_brightness_sub > 0)
      galaxy_2_brightness_sub -= dt * 0.0055; // скорость появления фона
    m_galaxy_2.update(Vec(vel.x * layer_speed_scale, layer_h_speed), dt);
    // ограничить движение фона по горизонтали
    auto w = m_galaxy_2.tilemap.get_original_w() - graphic::width;
    assert(w);
    m_galaxy_2.pos.x = std::clamp<real>(m_galaxy_2.pos.x, -w, 0);
  }

  inline void update_galaxy_3_layer(const Vec vel, Delta_time dt) {
    return_if ( !enable_galaxy_3_layer);
    m_galaxy_3.update(Vec(vel.x * layer_speed_scale, layer_h_speed), dt);
  }
}; // Impl

Level_space::Level_space(): impl {new_unique<Impl>()} {}
Level_space::~Level_space() {}
void Level_space::update(const Vec vel, Delta_time dt) {
  Level::update(vel, dt);
  impl->update(vel, dt);
}
void Level_space::draw(Image& dst) const { impl->draw(dst); }
Str Level_space::level_name() const { return Str{Level_space::NAME}; }
