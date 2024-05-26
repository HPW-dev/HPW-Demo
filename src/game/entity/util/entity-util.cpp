#include <cassert>
#include <algorithm>
#include "entity-util.hpp"
#include "phys.hpp"
#include "anim-ctx.hpp"
#include "graphic/animation/animation-manager.hpp"
#include "game/core/anims.hpp"
#include "game/entity/entity.hpp"
#include "game/entity/player.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/particle.hpp"
#include "game/core/canvas.hpp"
#include "util/error.hpp"
#include "util/hpw-util.hpp"
#include "util/math/mat.hpp"
#include "util/math/vec-util.hpp"

static Uid m_entity_uid = 0;

void add_anim(Entity& dst, CN<Str> anim_name) {
  assert(hpw::anim_mgr);
  dst.anim_ctx = {};
  auto anim = hpw::anim_mgr->find_anim(anim_name);
  assert(anim);
  dst.anim_ctx.set_anim(anim.get());
}

Particle* to_particle(Entity& src) {
  assert(src.type == ENTITY_TYPE(Particle));
  return ptr2ptr<Particle*>(&src);
}

Collidable* to_collidable(Entity& src) {
  assert(src.status.collidable);
  return ptr2ptr<Collidable*>(&src);
}

void bounce_off_screen(Entity& entity, double dt) {
  auto pos = entity.phys.get_pos();
  auto vel = entity.phys.get_vel();
  if (pos.x < 0) {
    pos.x = 0;
    vel.x *= -1;
  }
  if (pos.x > graphic::width) {
    pos.x = graphic::width;
    vel.x *= -1;
  }
  if (pos.y < 0) {
    pos.y = 0;
    vel.y *= -1;
  }
  if (pos.y > graphic::height) {
    pos.y = graphic::height;
    vel.y *= -1;
  }
  entity.phys.set_pos(pos);
  entity.phys.set_vel(vel);
} // bounce_off_screen

Anim_speed_slowdown::Anim_speed_slowdown(double slowndown)
: m_slowndown{slowndown} { assert(m_slowndown > 0); }

void Anim_speed_slowdown::operator()(Entity& entity, double dt) {
  auto scale = entity.anim_ctx.get_speed_scale();
  scale = std::max<double>(0, scale - m_slowndown * dt);
  entity.anim_ctx.set_speed_scale(scale);
}

Anim_speed_addiction::Anim_speed_addiction(double target_speed,
double min_ratio, double max_ratio)
: m_target_speed {target_speed}
, m_min_ratio {min_ratio}
, m_max_ratio {max_ratio}
{
  assert(m_max_ratio > 0);
  assert(m_min_ratio < m_max_ratio);
  assert(m_target_speed > 0);
}

void Anim_speed_addiction::operator()(Entity& entity, double dt) {
  /* определить на сколько объект быстрее чем m_max_speed и
  эту разницу применить к скорости воспроизведения анимации */
  auto speed = entity.phys.get_speed();
  auto ratio = speed / m_target_speed;
  entity.anim_ctx.set_speed_scale( std::clamp<double>(ratio, m_min_ratio, m_max_ratio) );
}

Rotate_speed_addiction::Rotate_speed_addiction(double target_speed,
double min_ratio, double max_ratio, double speed_scale, bool rot_right)
: m_target_speed {target_speed}
, m_min_ratio {min_ratio}
, m_max_ratio {max_ratio}
, m_speed_scale {speed_scale}
, m_rot_right {rot_right}
{
  assert(m_max_ratio > 0);
  assert(m_min_ratio < m_max_ratio);
  assert(m_target_speed > 0);
  assert(m_speed_scale > 0);
}

void Rotate_speed_addiction::operator()(Entity& entity, double dt) {
  /* определить на сколько объект быстрее чем m_max_speed и
  эту разницу применить к скорости вращения */
  auto speed = entity.phys.get_speed();
  auto ratio = speed / m_target_speed;
  ratio = std::clamp<double>(ratio, m_min_ratio, m_max_ratio);
  auto new_deg = entity.anim_ctx.get_default_deg() + ratio * m_speed_scale
    * (m_rot_right ? 1 : -1);
  new_deg = ring_deg(new_deg);
  entity.anim_ctx.set_default_deg( new_deg );
  entity.status.fixed_deg = true;
}

Kill_by_timeout::Kill_by_timeout(double timeout)
: m_timeout {timeout}
{ assert(timeout >= 0); }

void Kill_by_timeout::operator()(Entity& entity, double dt) {
  if (m_timeout <= 0) {
    entity.kill();
    return;
  }
  m_timeout -= dt;
}

Uid get_entity_uid() { return ++m_entity_uid; }

void clear_entity_uid() { m_entity_uid = 0; }

// @return false if flag success
inline bool check_flag_ignore_master(CN<Entity> a, CN<Entity> b) {
  return ( !(
    (a.status.ignore_master && a.master == std::addressof(b)) ||
    (b.status.ignore_master && b.master == std::addressof(a))
  ) );
}

// @return false if flag success
inline bool check_flag_ignore_enemy(CN<Entity> a, CN<Entity> b) {
  return ( !(
    (a.status.ignore_enemy && b.status.is_enemy) ||
    (b.status.ignore_enemy && a.status.is_enemy)
  ) );
}

// @return false if flag success
inline bool check_flag_ignore_player(CN<Entity> a, CN<Entity> b) {
  return ( !(
    (a.status.ignore_player && b.type == ENTITY_TYPE(Player)) ||
    (b.status.ignore_player && a.type == ENTITY_TYPE(Player))
  ) );
}

// @return false if flag success
inline bool check_flag_ignore_self_type(CN<Entity> a, CN<Entity> b) {
  return ( !(
    (a.status.ignore_self_type || b.status.ignore_self_type) && (a.type == b.type)
  ) );
}

// @return false if flag success
inline bool check_flag_ignore_bullet(CN<Entity> a, CN<Entity> b) {
  return ( !(
    (a.status.ignore_bullet && b.status.is_bullet) ||
    (b.status.ignore_bullet && a.status.is_bullet)
  ) );
}

bool cld_flag_compat(CN<Entity> a, CN<Entity> b) {
  // live и collidable уже проверен в QTree

  // если хотя бы один флаг стрельнёт, то объекты не смогут столкнуться
  return check_flag_ignore_master(a, b)
      && check_flag_ignore_self_type(a, b)
      && check_flag_ignore_player(a, b)
      && check_flag_ignore_enemy(a, b)
      && check_flag_ignore_bullet(a, b)
  ;
} // cld_flag_compat

real need_deg_to_target(CN<Entity> self, CN<Entity> target)
  { return need_deg_to_target(self, target.phys.get_pos()); }

real need_deg_to_target(CN<Entity> self, const Vec target) {
  auto motion = self.phys.get_pos() - target;
  return ring_deg( vec_to_deg(motion) - self.phys.get_deg() );
}

bool need_rotate_right(CN<Entity> self, const Vec target) {
  auto deg_to = need_deg_to_target(self, target);
  return deg_to > 180.0;
}

bool need_rotate_right(CN<Entity> self, CN<Entity> target)
  { return need_rotate_right(self, target.phys.get_pos()); }

real deg_to_target(CN<Entity> self, const Vec target)
  { return vec_to_deg( target - self.phys.get_pos() ); }

real deg_to_target(CN<Entity> self, CN<Entity> target)
  { return deg_to_target(self, target.phys.get_pos()); }

Vec predict(CN<Entity> self_, CN<Entity> target_, double dt) {
  cnauto self = self_.phys;
  cnauto target = target_.phys;

  if (self.get_speed() <= 0)
    return target.get_pos();

  // определить растояние до цели
  cauto dist = distance(target.get_pos(), self.get_pos());
  // узнать за сколько времени мы окажемся у цели
  const real t = dist / self.get_speed();
  // узнать где будет цель за это же время
  return target.get_pos() + normalize_stable(target.get_vel()) * target.get_speed() * t;
}

Timed_visible::Timed_visible(const double timeout) {
  assert(timeout > 0 && timeout < 100);
  m_timeout = timeout;
}

void Timed_visible::operator()(Entity& entity, double dt) {
  entity.status.no_motion_interp = true;
  
  if (m_timeout > 0) {
    m_timeout -= dt;
    entity.status.disable_render = false;
  } else {
    entity.status.disable_render = true;
  }
}
