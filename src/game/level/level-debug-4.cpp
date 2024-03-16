#include "level-debug-4.hpp"
#include "util/hpw-util.hpp"
#include "util/math/random.hpp"
#include "util/math/timer.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/animation/animation-manager.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/core/core.hpp"
#include "game/core/entities.hpp"
#include "game/core/levels.hpp"
#include "game/util/game-util.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/scatter.hpp"
#include "game/entity/player-dark.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/entity/particle.hpp"
#include "game/entity/collider/collider-qtree.hpp"
#include "game/level/level-manager.hpp"

struct Level_debug_4::Impl {
  Timer spawn_timer {0.005};

  inline Impl() {
    make_player();
    hpw::entity_mgr->set_collider(new_shared<Collider_qtree>(6, 1,
      graphic::canvas->X, graphic::canvas->Y));
  }

  inline void update(const Vec vel, double dt) {
    cauto player = hpw::entity_mgr->get_player();
    assert(player);
    cfor (_, spawn_timer.update(dt)) {
      cauto pos = player->phys.get_pos();
      auto it = hpw::entity_mgr->make(player, "bullet.placeholder.1", pos);
      //auto it = hpw::entity_mgr->make(player, "particle.123", pos);
      it->phys.set_speed( rndr(0.5_pps, 1.5_pps) );
      it->phys.set_force(0);
      it->phys.set_deg( rand_degree_stable() );
      if (it->status.collidable) {
        auto collidable = ptr2ptr<Collidable*>(it);
        collidable->set_explosion_name({});
      }
    }
  }

  inline void draw(Image& dst) const {
    dst.fill( Pal8::from_real(1.0 / 3.0, true) );
  }

  void make_player() {
    auto player_pos_from_prev_level = hpw::level_mgr->get_player_pos_from_prev_level();
    // если игрок где-то стоял до этого, то поставить его туда же, иначе в центр экрана
    auto pos = player_pos_from_prev_level ? player_pos_from_prev_level : get_screen_center();
    hpw::entity_mgr->make({}, "player.boo.dark", pos);
  }
}; // Impl

Level_debug_4::Level_debug_4(): impl {new_unique<Impl>()} {}
Level_debug_4::~Level_debug_4() {}
void Level_debug_4::update(const Vec vel, double dt) {
  Level::update(vel, dt);
  impl->update(vel, dt);
}
void Level_debug_4::draw(Image& dst) const { impl->draw(dst); }
