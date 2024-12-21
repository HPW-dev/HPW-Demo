#include "level.hpp"
#include "util/math/vec.hpp"
#include "util/math/timer.hpp"
#include "game/scene/scene-mgr.hpp"
#include "game/scene/scene-gameover.hpp"
#include "game/entity/player/player.hpp"
#include "game/core/entities.hpp"
#include "game/core/scenes.hpp"
#ifdef CLD_DEBUG
#include "game/core/debug.hpp"
#include "util/str-util.hpp"
#include "util/unicode.hpp"
#include "util/log.hpp"
#endif

struct Level::Impl {
  Level& _master;
  Timer death_timer {4.0}; // через это время засчитывается смерть игрока
  #ifdef CLD_DEBUG
  Timer cld_debug_timer {1};
  #endif

  inline explicit Impl(Level& master)
  : _master {master}
  {
    _master.on_player_death_action = [] {
      hpw::scene_mgr.add( new_shared<Scene_gameover>() );
    };
  }

  inline void update(const Vec vel, Delta_time dt) {
    #ifdef CLD_DEBUG
    if (cld_debug_timer.update(dt))
      print_collision_info();
    #endif

    process_player_death(dt);
  }

  // дебажный вывод количества столкновений
  inline static void print_collision_info() {
    #ifdef CLD_DEBUG
    Str txt;
    txt += "circle check: " + n2s<Str>(hpw::circle_checks);
    txt += ", poly: " + n2s<Str>(hpw::poly_checks);
    txt += ", collided: " + n2s<Str>(hpw::total_collided);
    hpw_log(txt << "\n");
    hpw::circle_checks = 0;
    hpw::poly_checks = 0;
    hpw::total_collided = 0;
    #endif
  }

  inline void process_player_death(const Delta_time dt) {
    ret_if (!_master.on_player_death_action);

    // перезапуск уровня, если игрок умер
    if (cauto player = hpw::entity_mgr->get_player(); player)
      if (!player->status.live)
        if (death_timer.update(dt)) // по завершению таймера
          _master.on_player_death_action();
  }
}; // Impl

Level::Level(): _impl {new_unique<Impl>(*this)} {}
Level::~Level() {}
void Level::update(const Vec vel, Delta_time dt) { _impl->update(vel, dt); }
