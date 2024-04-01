#include "level-tutorial.hpp"
#include "util/math/vec.hpp"
#include "util/math/random.hpp"
#include "util/unicode.hpp"
#include "util/hpw-util.hpp"
#include "game/entity/player.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/collider/collider-qtree.hpp"
#include "game/core/entities.hpp"
#include "game/core/canvas.hpp"
#include "game/core/common.hpp"
#include "game/core/levels.hpp"
#include "game/core/fonts.hpp"
#include "game/level/util/level-tasks.hpp"
#include "game/level/util/task-timed.hpp"
#include "game/level/level-manager.hpp"
#include "game/util/sync.hpp"
#include "game/util/game-util.hpp"
#include "game/util/keybits.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "graphic/effect/bg-pattern.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"

struct Level_tutorial::Impl {
  Level_tasks tasks {};
  utf32 bg_text {}; /// текст, который показывается на фоне

  inline explicit Impl() {
    hpw::shmup_mode = true;
    init_collider();
    make_player();
    init_tasks();
  }

  inline void update(const Vec vel, double dt) {
    execute_tasks(tasks, dt);
  }

  inline void draw(Image& dst) const {
    draw_bg(dst);
    draw_bg_text(dst);
  }

  inline void draw_upper_layer(Image& dst) const {}

  inline void make_player()
    { hpw::entity_mgr->make({}, "player.boo.dark", get_screen_center() + Vec(0, 100)); }

  inline void init_collider() {
    hpw::entity_mgr->set_collider(new_shared<Collider_qtree>(6, 1,
      graphic::width, graphic::height));
  }

  inline void init_tasks() {
    tasks = Level_tasks {
      // в начале ничего не происходит
      //Timed_task(3.3, [](double dt) { return false; }),
      Timed_task(9.0, Task_draw_motion_keys(this)),
      Spawner_border_bullet(this, 40, 0.6),
      [](double dt) { return false; }, // TODO заглушка
      &exit_from_level,
    }; // Level_tasks c-tor
  } // init_tasks

  inline void draw_bg(Image& dst) const {
    bgp_bit_1(dst, graphic::frame_count >> 2);
    apply_brightness(dst, -140);
  }

  inline void draw_bg_text(Image& dst) const {
    return_if(bg_text.empty());
    // текст в центре
    cauto center = get_screen_center();
    cauto text_sz = graphic::font->text_size(bg_text);
    cauto pos = center - (text_sz / 2);
    // затемнение фона текста
    cauto border = Vec(11, 11);
    cauto bg_shadow = Rect(pos - border, text_sz + border * 2);
    draw_rect_filled<&blend_158>(dst, bg_shadow, Pal8::black);
    // яркость текста сменяется
    cauto text_brightness = 150u + ((graphic::frame_count / 3u) % (255u - 150u));
    graphic::font->draw(dst, pos, bg_text, &blend_alpha, text_brightness);
  }

  /// Кнопки движения. Пропуск, когда всё нажато
  struct Task_draw_motion_keys {
    Impl* master {};
    bool key_l {};
    bool key_r {};
    bool key_u {};
    bool key_d {};

    inline explicit Task_draw_motion_keys (Impl* _master)
    : master {_master} {}

    inline bool operator()(double dt) {
      assert(master);
      master->draw_motion_keys();
      key_u |= is_pressed(hpw::keycode::up);
      key_d |= is_pressed(hpw::keycode::down);
      key_l |= is_pressed(hpw::keycode::left);
      key_r |= is_pressed(hpw::keycode::right);
      // выйти со сцены, если всё нажато
      return key_u && key_d && key_l && key_r;
    }
  }; // Task_draw_motion_keys

  inline void draw_motion_keys() {
    bg_text = get_locale_str("scene.tutorial.text.move_keys");
    #define KEY_TEXT(key_name) { \
      const bool pressed = is_pressed(hpw::keycode::key_name); \
      cauto scope_l = pressed ? U'{' : U' '; \
      cauto scope_r = pressed ? U'}' : U' '; \
      bg_text += U'\n' + get_locale_str("scene.input."#key_name) + U" - "; \
      bg_text += scope_l + hpw::keys_info.find(hpw::keycode::key_name)->name + scope_r; \
    }
    KEY_TEXT(left)
    KEY_TEXT(right)
    KEY_TEXT(up)
    KEY_TEXT(down)
    #undef KEY_TEXT
  } // draw_motion_keys

  /// выйти с уровня
  inline static bool exit_from_level(double dt) {
    hpw::level_mgr->finalize_level();
    return true;
  }

  /// спавнит пули по краям экрана, чтобы заставить игрока подвигаться
  struct Spawner_border_bullet {
    Impl* master {};
    uint count {}; /// когда станет 0, завершить
    Timer spwan_timer {}; /// скорость спавна пуль

    inline bool operator()(double dt) {
      master->draw_motion_keys();
      // надпись об уклонении
      master->bg_text += U"\n\n" + get_locale_str("scene.tutorial.text.wave_warning");

      cfor (_, spwan_timer.update(dt)) {
        --count;

        constexpr auto wave_speed = 1.7_pps;
        cauto w = graphic::width;
        cauto h = graphic::height;
        cauto player = hpw::entity_mgr->get_player();
        cont_if(!player);
        cauto px = player->phys.get_pos().x;
        cauto py = player->phys.get_pos().y;

        struct Wave_dir {
          Vec rnd_st {};
          Vec rnd_ed {};
          Vec vel {};
        };
        Vector<Wave_dir> wave_dir_table {
          Wave_dir {.rnd_st={10, -20},  .rnd_ed={w-10, -20},  .vel={0, wave_speed}},
          Wave_dir {.rnd_st={10, h-10}, .rnd_ed={w-10, h-10}, .vel={0, -wave_speed}},
          Wave_dir {.rnd_st={-20, 10},  .rnd_ed={-20, h-10},  .vel={wave_speed, 0}},
          Wave_dir {.rnd_st={w-10, 10}, .rnd_ed={w-10, h-10}, .vel={-wave_speed, 0}},
          // нацелены на игрока:
          Wave_dir {.rnd_st={px, -20},  .rnd_ed={px, -20},  .vel={0, wave_speed}},
          Wave_dir {.rnd_st={px, h-10}, .rnd_ed={px, h-10}, .vel={0, -wave_speed}},
          Wave_dir {.rnd_st={-20, py},  .rnd_ed={-20, py},  .vel={wave_speed, 0}},
          Wave_dir {.rnd_st={w-10, py}, .rnd_ed={w-10, py}, .vel={-wave_speed, 0}},
        };
        cnauto wave_dir = wave_dir_table.at( rndu(wave_dir_table.size()-1) );

        cauto pos = get_rand_pos_safe(wave_dir.rnd_st.x, wave_dir.rnd_st.y,
          wave_dir.rnd_ed.x, wave_dir.rnd_ed.y);
        auto wave = hpw::entity_mgr->make({}, "bullet.sphere.wave", pos);
        wave->phys.set_vel(wave_dir.vel);
        wave->anim_ctx.set_speed_scale(0.8);
      }
      return count == 0 || count >= 9999;
    } // op ()

    inline explicit Spawner_border_bullet(Impl* _master, const uint _count,
    const real _spwan_timer)
    : master{_master}
    , count{_count}
    , spwan_timer{_spwan_timer}
    {
      assert(_master);
      assert(count > 0 && count < 9999);
      assert(_spwan_timer > 0 && _spwan_timer < 100);
    }
  }; // Spawner_border_bullet
}; // Impl

Level_tutorial::Level_tutorial(): impl {new_unique<Impl>()} {}
Level_tutorial::~Level_tutorial() {}
void Level_tutorial::update(const Vec vel, double dt) {
  Level::update(vel, dt);
  impl->update(vel, dt);
}
void Level_tutorial::draw(Image& dst) const { impl->draw(dst); }
void Level_tutorial::draw_upper_layer(Image& dst) const { impl->draw_upper_layer(dst); }
