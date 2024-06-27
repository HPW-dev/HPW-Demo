#ifdef DEBUG
  #include <iostream>
#endif
#include "level-debug-3.hpp"
#include "util/hpw-util.hpp"
#include "util/math/random.hpp"
#include "util/math/timer.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/core/core.hpp"
#include "game/core/entities.hpp"
#include "game/util/game-util.hpp"
#include "game/core/levels.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/scatter.hpp"
#include "game/entity/player/player-dark.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/entity/particle.hpp"
#include "game/entity/collider/collider-qtree.hpp"
//#include "game/entity/collider/collider-simple.hpp"

struct Level_debug_3::Impl {
  Image bg {};
  Timer blt_tmr {0.001};
  Timer expl_tmr {0.4};
  Timer spawn_tmr {0.4242};
  Timer spawn_bouncer_tmr {3};

  inline Impl()
  : bg (graphic::width, graphic::height)
  {}

  inline void make_bg() {
    // сделать постоянный шум на фоне
    for (nauto pix: bg)
      pix = Pal8::from_real(rndr(0, 0.166));

    // сетка
    cfor (y, bg.Y)
    cfor (x, bg.X) {
      auto grid_sz = 32;
      auto grid_color = Pal8::from_real(0.2);
      if ((x % grid_sz == 0) || (y % grid_sz == 0))
        bg(x, y) = grid_color;
    }

    to_red(bg);
    //apply_invert(bg);

    // прогличить
    for (int i = 1; i < bg.size; ++i)
      if ((rndu_fast() & 3) == 0)
        bg[i] = blend_add_safe(bg[i-1], bg[i]);
  } // make_bg

}; // Impl

Level_debug_3::Level_debug_3()
: impl {new_unique<Impl>()}
{
  #ifdef DEBUG
    std::cerr << "start debug cast\n";
  #endif
  make_player();
  make_dummy();
  impl->make_bg();
  hpw::entity_mgr->set_collider(new_shared<Collider_qtree>(7, 1, graphic::canvas->X, graphic::canvas->Y));
  //hpw::entity_mgr->set_collider(new_shared<Collider_simple>());
} // c-tor

Level_debug_3::~Level_debug_3() {}

void Level_debug_3::update(const Vec vel, Delta_time dt) {
  Level::update(vel, dt);

  // спавн волн пуль
  #if 1
  spawn_bullets(dt);
  #endif

  // создавать взрывные волны в случайных местах
  #if 0
  cfor (_, impl->expl_tmr.update(dt)) {
    hpw::entity_mgr->add_scatter( Scatter {
      .pos = Vec(rndr(0, graphic::canvas->X), rndr(0, graphic::canvas->Y)),
      .range = 100,
      .power = 10_pps,
      .type = Scatter::Type::outside
    });
  }
  #endif

  // пули летят вверх
  #if 0
  cfor (_, impl->blt_tmr.update(dt))
  //cfor (__, 7)
  {
    auto blt = hpw::entity_mgr->allocate<Collidable>();
    add_anim(*blt, "anim.bullet.small.blinker.1");
    const Vec pos (
      graphic::width / 2,
      graphic::height / 2
    );
    blt->set_pos(pos + Vec(30, 100));
    blt->phys.set_speed( pps(rndr(4, 20)) );
    blt->phys.set_deg(-90 + rndr(-1, 1) * 10);
    blt->anim_ctx.randomize_cur_frame_safe();
    blt->status.layer_up = true;
    blt->move_update_callback( Kill_by_timeout() );
    blt->status.ignore_self_type = true;
  }
  #endif

  /*if ((hpw::game_updates_safe % 100) == 0)
    save_dbg_info();*/

} // update

void Level_debug_3::draw(Image& dst) const {
  draw_bg(dst);

  auto player = hpw::entity_mgr->get_player();
  assert(player);

  // нарисовать корды игрока
  #if 0
  cauto pos = player->phys.get_pos();
  cauto vel = player->phys.get_vel();
  cauto spd = player->phys.get_speed();
  cauto frc = player->phys.get_force();
  utf32 pos_info = 
    U"pos: " + n2s<utf32>(pos.x) + U", " + n2s<utf32>(pos.y) + U'\n' +
    U"vel: " + n2s<utf32>(vel.x) + U", " + n2s<utf32>(vel.y) + U'\n' +
    U"spd: " + n2s<utf32>(spd) + U'\n' +
    U"frc: " + n2s<utf32>(frc) ;
  graphic::font->draw(dst, {5, 5}, pos_info);
  #endif
} // draw

void Level_debug_3::draw_bg(Image& dst) const {
  insert_fast(dst, impl->bg);
}

void Level_debug_3::make_player() {
  auto player_pos_from_prev_level = hpw::level_mgr->player_prev_lvl_pos();
  // если игрок где-то стоял до этого, то поставить его туда же, иначе в центр экрана
  auto pos = player_pos_from_prev_level.not_zero() ? player_pos_from_prev_level : get_screen_center();
  hpw::entity_mgr->make({}, "player.boo.dark", pos);
}

void Level_debug_3::make_dummy() {
  
  auto player = hpw::entity_mgr->get_player();
  assert(player);

  // рандомно накидать вокруг игрока объекты, чтобы можно было в них врезаться
  #if 0
  cfor (_, 8) {
    auto hittest = hpw::entity_mgr->allocate<Collidable>();
    constexpr std::array<Cstr, 5> names {
      "anim.bullet.delme.1",
      "anim.bullet.delme.2",
      "anim.enemy.delme",
      "anim.bullet.small.blinker.2",
      "anim.bullet.small.blinker.1"
    };
    add_anim(*hittest, names.at( rndu(names.size() - 1) ));
    auto pos = rand_normalized_stable() * 150.0;
    hittest->set_pos(pos + player->phys.get_pos());
    hittest->phys.set_deg( rand_degree_stable() );
    hittest->anim_ctx.randomize_cur_frame_safe();
    hittest->status.layer_up = true;
    hittest->status.ignore_scatter = true;
  }
  #endif

  // для теста размытия
  #if 0
  cfor (i, 8) {
    auto entity = hpw::entity_mgr->make({}, "particle.frac.metall.1",
      player->phys.get_pos());
    entity->phys.set_deg( rand_degree_stable() );
    entity->phys.set_speed( 1_pps + i * 10_pps );
    entity->phys.set_force(0);
    entity->status.rnd_deg = true;
    entity->status.kill_by_timeout = false;
    entity->anim_ctx.randomize_cur_frame_safe();
    entity->move_update_callback(&bounce_off_screen);
  }
  #endif

  // наспавнить сетку частиц для расталкивания
  #if 0
  cfor (y, 17)
  cfor (x, 22) {
    uint sz = 24;
    Vec pos(x * sz, y * sz);
    auto entity = hpw::entity_mgr->make({}, "particle.frac.metall.1", pos);
    entity->status.kill_by_timeout = false;
    //entity->phys.set_rot_spd( rndr(0, 7_pps) );
    //entity->phys.set_rot_fc( rndr(0.5_pps, 3_pps) );
    entity->phys.set_force( 5_pps );
    entity->move_update_callback( &bounce_off_screen );
    entity->move_update_callback( Anim_speed_addiction(5_pps, 0, 3) );
    entity->move_update_callback( Rotate_speed_addiction(5_pps, 0, 3, 8,
      scast<bool>(rndb() & 1)) );
  }
  #endif
  
} // make_dummy

void Level_debug_3::spawn_bullets(const Delta_time dt) {
  cfor (_, impl->spawn_tmr.update(dt)) {
    real spawn_offset = rndr(-20, 20);
    for (int i = -6; i < 5; ++i) {
      Vec pos(graphic::canvas->X/2.0, -20);
      auto blt = hpw::entity_mgr->make({}, "bullet.sphere.red", pos);
      real deg = 90;
      // сменяет узор на рандомный
      deg += (hpw::game_updates_safe % (120 * 3) < 120 * 1.5)
        ? spawn_offset + i * rndr(0, 30)
        : spawn_offset + i * 15;
      blt->phys.set_speed( 3.333_pps );
      blt->phys.set_deg(deg);
    }
  } // for spawn_tmr

  cfor (_, impl->spawn_bouncer_tmr.update(dt)) {
    Vec pos(graphic::canvas->X/2.0, -20);
    auto blt = hpw::entity_mgr->make({}, "bullet.sphere.gray", pos);
    blt->phys.set_speed( 4_pps );
    blt->phys.set_deg( rand_degree_stable() );
    blt->move_update_callback(&bounce_off_screen);
    blt->move_update_callback(Kill_by_timeout(5));
    blt->anim_ctx.blend_f = &blend_rotate_x16_safe;
  }
} // spawn_bullets

void Level_debug_3::save_dbg_info() {
  #ifdef DEBUG
  for (uint idx {}; auto entity: hpw::entity_mgr->get_entities()) {
    if (entity->status.live) {
      std::cerr << "game_update: " << hpw::game_updates_safe << "\n";
      std::cerr << "entity [" << idx << "]:\n";
      std::cerr <<"  pos: " << to_str(entity->phys.get_pos()) << '\n';
      std::cerr <<"  vel: " << to_str(entity->phys.get_vel()) << '\n';
      std::cerr <<"  spd: " << entity->phys.get_speed() << '\n';
      std::cerr <<"  deg: " << entity->phys.get_deg() << '\n';
      std::cerr <<"  acc: " << entity->phys.get_accel() << '\n';
      std::cerr <<"  frc: " << entity->phys.get_force() << '\n';
      std::cerr << '\n';
    }
    ++idx;
  }
  std::cerr << std::endl;
  #endif
}
