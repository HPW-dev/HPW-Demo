#include "level.hpp"
#include "util/math/vec.hpp"
#include "util/math/timer.hpp"
#include "game/scene/scene-manager.hpp"
#include "game/scene/scene-gameover.hpp"
#include "game/entity/player.hpp"
#include "game/core/entities.hpp"
#include "game/core/scenes.hpp"
#ifdef CLD_DEBUG
#include "game/core/debug.hpp"
#include "util/str-util.hpp"
#include "util/unicode.hpp"
#include "util/log.hpp"
#endif

struct Level::Impl {
  Level* master {};
  Timer death_timer {4.0}; // через это время засчитывается смерть игрока
  #ifdef CLD_DEBUG
  Timer cld_debug_timer {1};
  #endif

  inline explicit Impl(Level* _master): master {_master} {}
  ~Impl() {}

  inline void update(const Vec vel, Delta_time dt) {
    #ifdef CLD_DEBUG
    if (cld_debug_timer.update(dt))
      print_collision_info();
    #endif
    on_player_death(dt);
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

  inline void on_player_death(const Delta_time dt) {
    return_if (!master->on_player_death_action);

    // перезапуск уровня, если игрок умер
    if (cauto player = hpw::entity_mgr->get_player(); player)
      if ( !player->status.live)
        if (death_timer.update(dt)) // по завершению таймера
          master->on_player_death_action();
  }
}; // Impl

void Level::update(const Vec vel, Delta_time dt) { impl->update(vel, dt); }
void Level::on_player_death(const Delta_time dt) { impl->on_player_death(dt); }
Level::~Level() {}

Level::Level(): impl {new_unique<Impl>(this)} {
  on_player_death_action = [] {
    hpw::scene_mgr->add( new_shared<Scene_gameover>() );
  };
}
