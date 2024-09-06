#include <cassert>
#include <algorithm>
#include "entity-util.hpp"
#include "phys.hpp"
#include "anim-ctx.hpp"
#include "game/core/common.hpp"
#include "game/core/anims.hpp"
#include "game/core/entities.hpp"
#include "game/core/canvas.hpp"
#include "game/core/core.hpp"
#include "game/entity/entity.hpp"
#include "game/entity/player/player.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/particle.hpp"
#include "game/entity/collider/collider-2d-tree.hpp"
#include "util/error.hpp"
#include "util/hpw-util.hpp"
#include "util/math/mat.hpp"
#include "util/math/vec-util.hpp"
#include "graphic/animation/anim.hpp"
#include "graphic/animation/frame.hpp"
#include "graphic/image/image.hpp"
#include "graphic/sprite/sprite.hpp"

static Uid m_entity_uid = 0;

void add_anim(Entity& dst, cr<Str> anim_name) {
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

void bounce_off_screen(Entity& entity, Delta_time dt) {
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

Anim_speed_slowdown::Anim_speed_slowdown(const Delta_time slowndown)
: m_slowndown{slowndown} { assert(m_slowndown > 0); }

void Anim_speed_slowdown::operator()(Entity& entity, const Delta_time dt) {
  auto scale = entity.anim_ctx.get_speed_scale();
  scale = std::max<Delta_time>(0, scale - m_slowndown * dt);
  entity.anim_ctx.set_speed_scale(scale);
}

Anim_speed_addiction::Anim_speed_addiction(Delta_time target_speed,
Delta_time min_ratio, Delta_time max_ratio)
: m_target_speed {target_speed}
, m_min_ratio {min_ratio}
, m_max_ratio {max_ratio}
{
  assert(m_max_ratio > 0);
  assert(m_min_ratio < m_max_ratio);
  assert(m_target_speed > 0);
}

void Anim_speed_addiction::operator()(Entity& entity, Delta_time dt) {
  /* определить на сколько объект быстрее чем m_max_speed и
  эту разницу применить к скорости воспроизведения анимации */
  auto speed = entity.phys.get_speed();
  auto ratio = speed / m_target_speed;
  entity.anim_ctx.set_speed_scale( std::clamp<Delta_time>(ratio, m_min_ratio, m_max_ratio) );
}

Rotate_speed_addiction::Rotate_speed_addiction(Delta_time target_speed,
Delta_time min_ratio, Delta_time max_ratio, Delta_time speed_scale, bool rot_right)
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

void Rotate_speed_addiction::operator()(Entity& entity, Delta_time dt) {
  /* определить на сколько объект быстрее чем m_max_speed и
  эту разницу применить к скорости вращения */
  auto speed = entity.phys.get_speed();
  auto ratio = speed / m_target_speed;
  ratio = std::clamp<Delta_time>(ratio, m_min_ratio, m_max_ratio);
  auto new_deg = entity.anim_ctx.get_default_deg() + ratio * m_speed_scale
    * (m_rot_right ? 1 : -1);
  new_deg = ring_deg(new_deg);
  entity.anim_ctx.set_default_deg( new_deg );
  entity.status.fixed_deg = true;
}

Kill_by_timeout::Kill_by_timeout(const Delta_time timeout)
: m_timeout {timeout}
{ assert(timeout >= 0); }

void Kill_by_timeout::operator()(Entity& entity, Delta_time dt) {
  if (m_timeout <= 0) {
    entity.kill();
    return;
  }
  m_timeout -= dt;
}

Uid get_entity_uid() { return ++m_entity_uid; }

void clear_entity_uid() { m_entity_uid = 0; }

// @return false if flag success
inline bool check_flag_ignore_master(cr<Entity> a, cr<Entity> b) {
  return ( !(
    (a.status.ignore_master && a.get_master() == std::addressof(b)) ||
    (b.status.ignore_master && b.get_master() == std::addressof(a))
  ) );
}

// @return false if flag success
inline bool check_flag_ignore_enemy(cr<Entity> a, cr<Entity> b) {
  return ( !(
    (a.status.ignore_enemy && b.status.is_enemy) ||
    (b.status.ignore_enemy && a.status.is_enemy)
  ) );
}

// @return false if flag success
inline bool check_flag_ignore_player(cr<Entity> a, cr<Entity> b) {
  return ( !(
    (a.status.ignore_player && b.type == ENTITY_TYPE(Player)) ||
    (b.status.ignore_player && a.type == ENTITY_TYPE(Player))
  ) );
}

// @return false if flag success
inline bool check_flag_ignore_self_type(cr<Entity> a, cr<Entity> b) {
  return ( !(
    (a.status.ignore_self_type || b.status.ignore_self_type) && (a.type == b.type)
  ) );
}

// @return false if flag success
inline bool check_flag_ignore_bullet(cr<Entity> a, cr<Entity> b) {
  return ( !(
    (a.status.ignore_bullet && b.status.is_bullet) ||
    (b.status.ignore_bullet && a.status.is_bullet)
  ) );
}

bool cld_flag_compat(cr<Entity> a, cr<Entity> b) {
  // live и collidable уже проверен в QTree

  // если хотя бы один флаг стрельнёт, то объекты не смогут столкнуться
  return check_flag_ignore_master(a, b)
      && check_flag_ignore_self_type(a, b)
      && check_flag_ignore_player(a, b)
      && check_flag_ignore_enemy(a, b)
      && check_flag_ignore_bullet(a, b)
  ;
} // cld_flag_compat

real need_deg_to_target(cr<Entity> self, cr<Entity> target)
  { return need_deg_to_target(self, target.phys.get_pos()); }

real need_deg_to_target(cr<Entity> self, const Vec target) {
  auto motion = self.phys.get_pos() - target;
  return ring_deg( vec_to_deg(motion) - self.phys.get_deg() );
}

bool need_rotate_right(cr<Entity> self, const Vec target) {
  auto deg_to = need_deg_to_target(self, target);
  return deg_to > 180.0;
}

bool need_rotate_right(cr<Entity> self, cr<Entity> target)
  { return need_rotate_right(self, target.phys.get_pos()); }

real deg_to_target(const Vec self, const Vec target)
  { return vec_to_deg(target - self); }

// простая версия поиска упреждающей точки
static Vec rough_predict(cr<Phys> self, cr<Phys> target) {
  return_if (self.get_speed() <= 0, target.get_pos());
  // определить растояние до цели
  cauto dist = distance(target.get_pos(), self.get_pos());
  // узнать за сколько времени мы окажемся у цели
  const real t = dist / self.get_speed();
  // узнать где будет цель за это же время
  return target.get_pos() + normalize_stable(target.get_vel()) * target.get_speed() * t;
}

Vec predict(cr<Phys> self, cr<Phys> target) {
  cauto start = self.get_pos();
  auto src = self;
  auto dst = target;
  dst.set_force(0); // иначе в игрока невозжможно будет попасть
  constexpr uint TIMEOUT = 1'000; // макс число шагов

  // двигать объекты пока src не достигнет радиуса попадания в dst
  cfor (_, TIMEOUT) {
    src.update(hpw::target_update_time);
    dst.update(hpw::target_update_time);
    if (length(src.get_pos() - start) >= length(dst.get_pos() - start))
      return dst.get_pos();
  }

  // если ничего не вышло, провести грубый расчёт
  return rough_predict(self, target);
}

Timed_visible::Timed_visible(const Delta_time timeout) {
  assert(timeout > 0 && timeout < 100);
  m_timeout = timeout;
}

void Timed_visible::operator()(Entity& entity, Delta_time dt) {
  entity.status.no_motion_interp = true;
  
  if (m_timeout > 0) {
    m_timeout -= dt;
    entity.status.disable_render = false;
  } else {
    entity.status.disable_render = true;
  }
}

void kill_if_master_death(Entity& entity, Delta_time dt) {
  if (!entity.get_master()) { // если нет инфы о создателе
    entity.kill();
  } elif (!entity.get_master()->status.live) { // если создатель умер
    entity.kill();
  }
}
Timed_kill_if_master_death::Timed_kill_if_master_death(
const Delta_time delay): m_delay {delay} {}

void Timed_kill_if_master_death::operator()
(Entity& entity, Delta_time dt) {
  if (m_master_death) {
    if ((m_delay -= dt) <= 0)
      entity.kill();
  }

  return_if(entity.get_master() && entity.get_master()->status.live);
  m_master_death = true;
}

Bound_off_screen::Bound_off_screen(cr<Entity> src) {
  // получить размеры игрока
  auto anim = src.get_anim();
  assert(anim);
  auto frame = anim->get_frame(0);
  assert(frame);
  auto direct = frame->get_direct(0);
  assert(direct);
  auto sprite = direct->sprite.lock();
  assert(sprite);
  auto image = sprite->image();
  Vec player_sz(image.X, image.Y);
  screen_lu = Vec(
    -1 * direct->offset.x,
    -1 * direct->offset.y
  );
  screen_rd = Vec(
    graphic::width  - player_sz.x - direct->offset.x,
    graphic::height - player_sz.y - direct->offset.y
  );
} // Bound_off_screen c-tor

void Bound_off_screen::operator()(Entity& dst, const Delta_time dt) {
  auto pos = dst.phys.get_pos();
  bool decrease_speed {false};
  if (pos.x < screen_lu.x)
    { pos.x = screen_lu.x; decrease_speed = true; }
  if (pos.x >= screen_rd.x)
    { pos.x = screen_rd.x-1; decrease_speed = true; }
  if (pos.y < screen_lu.y)
    { pos.y = screen_lu.y; decrease_speed = true; }
  if (pos.y >= screen_rd.y)
    { pos.y = screen_rd.y-1; decrease_speed = true; }
  // это фиксит быстрое движение при отталкивании
  if (decrease_speed)
    dst.phys.set_speed( dst.phys.get_speed() * 0.25 );
  dst.phys.set_pos(pos);
}

void set_default_collider() {
  assert(hpw::entity_mgr);
  hpw::entity_mgr->set_collider( new_shared<Collider_2d_tree>() );
}

Rotate_to_target::Rotate_to_target(cr<Target_getter> target_getter, const real rotate_speed)
: _target_getter {target_getter}
, _rotate_speed {rotate_speed}
{
  assert(_target_getter);
  assert(_rotate_speed > 0);
}

void Rotate_to_target::operator()(Entity& self, const Delta_time dt) {
  cauto tgt = _target_getter();
  cauto deg = deg_to_target(self.phys.get_pos(), tgt);
  self.phys.set_rot_spd(_rotate_speed);
  
  cauto deg_diff = ring_deg(deg - self.phys.get_deg());
  self.phys.set_invert_rotation(deg_diff > 180);
}

bool bound_check_for_collisions(cr<Collidable> other) {
  assert(hpw::shmup_mode); // для другого режима добавь новые константы
  cauto bound = hpw::shmup_bound_for_collisions;
  cauto pos = other.phys.get_pos();

  return
    pos.x > -bound &&
    pos.x < graphic::width + bound &&
    pos.y > -bound &&
    pos.y < graphic::height + bound;
}
