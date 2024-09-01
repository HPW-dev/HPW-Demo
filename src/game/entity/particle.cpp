#include <cassert>
#include "particle.hpp"
#include "game/core/graphic.hpp"
#include "game/util/sync.hpp"
#include "util/math/vec.hpp"

Particle::Particle()
: Entity(GET_SELF_TYPE)
, lifetime {}
{}

void Particle::update(const Delta_time dt) {
  Entity::update(dt);
  lifetime_update(dt);
}

void Particle::lifetime_update(const Delta_time dt) {
  // уменьшить время жизни, если уже на нуле - умереть
  if (lifetime > 0) {
    lifetime -= dt;
  } else {
    if (status.kill_by_timeout)
      status.killme = true;
  }
}

void Particle::set_lifetime(Delta_time new_lifetime, bool enable_flag) {
  assert(new_lifetime > 0);
  assert(new_lifetime < 9999);
  lifetime = new_lifetime;
  status.kill_by_timeout = enable_flag;
}

void Particle::draw(Image& dst, const Vec offset) const {
  // чтобы мигать спрайтами при лагах
  return_if (
    graphic::render_lag &&
    graphic::blink_particles &&
    ((graphic::frame_count ^ this->uid) & 1)
  );

  Entity::draw(dst, offset);
}
