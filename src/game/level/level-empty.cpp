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

#include "game/core/sounds.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/timer.hpp"
#include "util/math/random.hpp"
#include "util/error.hpp"

// TODO del:
class Bullet_maker final {
public:
  inline explicit Bullet_maker(const real delay, const real bullet_speed)
  : _delay(delay), _bullet_speed{bullet_speed} {}

  inline void operator()(Entity& self, const Delta_time dt) {
    cfor (_, _delay.update(dt)) {
      auto blt = hpw::entity_mgr->make(&self, "bullet.placeholder.1", self.phys.get_pos());
      blt->phys.set_speed(rndr(2.0_pps, _bullet_speed));
      blt->add_update_cb(Sound_attached(*blt, "sfx/effect/gas.flac", true));
      // при столкновении звук взрыва
      blt->add_kill_cb([](Entity& ent) {
        hpw::sound_mgr->play("sfx/explosion/small.flac", to_sound_pos(ent.phys.get_pos()));
      });
    }
  }
  
private:
  Timer _delay {};
  real _bullet_speed {};
};

struct Level_empty::Impl {
  inline explicit Impl() {
    set_default_collider();

    // TODO DEL:

    /*auto b = hpw::entity_mgr->make({}, "enemy.tutorial", get_screen_center());
    b->phys.set_vel({-3.0_pps, 0});
    b->add_update_cb([](Entity& self, const Delta_time dt) {
      bool respawn {};
      respawn |= self.phys.get_pos().x <= 0;
      respawn |= self.phys.get_pos().y <= 0;
      respawn |= self.phys.get_pos().x > graphic::width;
      respawn |= self.phys.get_pos().y > graphic::height;

      if (respawn) {
        self.phys.set_pos(rnd_screen_pos_safe());
        self.phys.set_deg(rand_degree_stable());
        self.phys.set_speed(3.0_pps);
      }
    });
    cauto b_uid = b->uid;*/

    auto b = hpw::entity_mgr->make({}, "player.boo.dark", get_screen_center() + Vec(0, 100));
    cauto b_uid = b->uid;

    //constexpr real BULLET_SPEED = 7.0_pps;
    constexpr real BULLET_SPEED = 20.0_pps;

    auto a = hpw::entity_mgr->make({}, "enemy.tutorial", get_screen_center());
    a->add_update_cb(Rotate_to_target([=]{
      cauto ent = hpw::entity_mgr->find(b_uid);
      Phys bullet_phys;
      bullet_phys.set_pos(a->phys.get_pos());
      bullet_phys.set_speed(BULLET_SPEED);
      return ent ? predict(bullet_phys, ent->phys) : rnd_screen_pos_safe();
    }, 300));
    //a->add_update_cb(Bullet_maker(0.12, BULLET_SPEED));
    a->add_update_cb(Bullet_maker(0.75, BULLET_SPEED));
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
