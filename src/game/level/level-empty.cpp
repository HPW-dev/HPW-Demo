#include "level-empty.hpp"
#include "game/core/canvas.hpp"
#include "game/core/core.hpp"
#include "game/core/entities.hpp"
#include "game/core/levels.hpp"
#include "game/core/tasks.hpp"
#include "game/core/common.hpp"
#include "game/util/game-util.hpp"
#include "game/util/task-util.hpp"
#include "game/entity/util/entity-util.hpp"
#include "util/hpw-util.hpp"
#include "util/math/timer.hpp"
#include "util/math/mat.hpp"

class Bullet_maker final {
public:
  inline explicit Bullet_maker(const real delay, const real bullet_speed)
  : _delay(delay), _bullet_speed{bullet_speed} {}

  inline void operator()(Entity& self, const Delta_time dt) {
    cfor (_, _delay.update(dt)) {
      auto blt = hpw::entity_mgr->make(&self, "bullet.placeholder.2", self.phys.get_pos());
      blt->phys.set_speed(_bullet_speed);
    }
  }
  
private:
  Timer _delay {};
  real _bullet_speed {};
};

struct Level_empty::Impl {
  inline explicit Impl() {
    set_default_collider();

    // TODO del:
    auto b = hpw::entity_mgr->make({}, "enemy.tutorial", get_screen_center() + Vec(200, 50));
    //b->phys.set_vel({-2.0_pps, 1.0_pps});
    b->phys.set_vel({-2.0_pps, 0});
    //b->phys.set_vel({-1.0_pps, 0.5_pps});
    b->add_update_cb([](Entity& self, const Delta_time dt) {
      if (self.phys.get_pos().x <= 0)
        self.phys.set_pos({graphic::width, self.phys.get_pos().y});
    });

    cauto b_uid = b->uid;
    constexpr real BULLET_SPEED = 3.0_pps;

    //auto b = hpw::entity_mgr->make({}, "player.boo.dark", get_screen_center());
    //cauto b_uid = b->uid;

    auto a = hpw::entity_mgr->make({}, "enemy.tutorial", get_screen_center() - Vec(0, 100));
    a->add_update_cb(Rotate_to_target([=]{
      cauto ent = hpw::entity_mgr->find(b_uid);
      Phys bullet_phys;
      bullet_phys.set_pos(a->phys.get_pos());
      bullet_phys.set_speed(BULLET_SPEED);
      //return ent ? ent->phys.get_pos() : rnd_screen_pos_safe();
      return ent ? predict(bullet_phys, ent->phys) : rnd_screen_pos_safe();
    }, 100));
    a->add_update_cb(Bullet_maker(0.25, BULLET_SPEED));
  }

  inline void update(const Vec vel, Delta_time dt) {
    //...
  }

  inline void draw(Image& dst) const {
    dst.fill( Pal8::from_real(1.0 / 3.0, true) );
  }
}; // Impl

Level_empty::Level_empty(): impl {new_unique<Impl>()} {}
Level_empty::~Level_empty() {}
void Level_empty::update(const Vec vel, Delta_time dt) {
  Level::update(vel, dt);
  impl->update(vel, dt);
}
void Level_empty::draw(Image& dst) const { impl->draw(dst); }
Str Level_empty::level_name() const { return Str{Level_empty::NAME}; }
