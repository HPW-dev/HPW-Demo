#include "level-debug-bullets.hpp"
#include "game/core/canvas.hpp"
#include "game/core/entities.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/collider/collider-qtree.hpp"
#include "graphic/image/image.hpp"
#include "util/math/vec.hpp"
#include "util/math/timer.hpp"
#include "util/hpw-util.hpp"

struct Level_debug_bullets::Impl {
  Timer shoot_timer {0.5};

  inline Impl() {
    hpw::entity_mgr->set_collider(new_shared<Collider_qtree>(6, 1,
      graphic::canvas->X, graphic::canvas->Y));
  }

  inline void update(const Vec vel, double dt) {
    cfor(_, shoot_timer.update(dt)) {
      auto a = hpw::entity_mgr->make({}, "bullet.placeholder.1", Vec(300, -30));
      auto b = hpw::entity_mgr->make({}, "bullet.placeholder.1", Vec(-30, 300));
      cauto speed = 120_pps;
      a->phys.set_speed(speed);
      b->phys.set_speed(speed);
      a->phys.set_deg(90);
      b->phys.set_deg(0);
      a->status.ignore_bullet = false;
      b->status.ignore_bullet = false;
      a->status.ignore_self_type = false;
      b->status.ignore_self_type = false;
      a->status.ignore_scatter = true;
      b->status.ignore_scatter = true;
    }
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::from_real(0.25, true));
  }

}; // Impl

Level_debug_bullets::Level_debug_bullets(): impl {new_unique<Impl>()} {}
Level_debug_bullets::~Level_debug_bullets() {}
void Level_debug_bullets::update(const Vec vel, double dt) {
  Level::update(vel, dt);
  impl->update(vel, dt);
}
void Level_debug_bullets::draw(Image& dst) const { impl->draw(dst); }