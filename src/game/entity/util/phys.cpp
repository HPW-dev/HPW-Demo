#include <algorithm>
#include <cassert>
#include "phys.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/mat.hpp"
#include "util/hpw-util.hpp"

CN<decltype(Phys::m_old_pos)> Phys::get_old_pos() const { return m_old_pos; }
CN<decltype(Phys::m_pos)> Phys::get_pos() const { return m_pos; }
CN<decltype(Phys::m_deg)> Phys::get_deg() const { return m_deg; }
CN<decltype(Phys::m_accel)> Phys::get_accel() const { return m_accel; }
CN<decltype(Phys::m_force)> Phys::get_force() const { return m_force; }
CN<decltype(Phys::m_speed)> Phys::get_speed() const { return m_speed; }
CN<decltype(Phys::m_rotate_speed)> Phys::get_rot_spd() const { return m_rotate_speed; }
CN<decltype(Phys::m_rotate_accel)> Phys::get_rot_ac() const { return m_rotate_accel; }
CN<decltype(Phys::m_rotate_force)> Phys::get_rot_fc() const { return m_rotate_force; }
Vec Phys::get_direction() const { return normalize_stable(get_vel()); }
void Phys::set_force( CN<decltype(m_force)> _force) { m_force = std::max<decltype(m_force)>(0, _force); }
CN<decltype(Phys::m_invert_rotation)> Phys::get_invert_rotation() const { return m_invert_rotation; }
void Phys::set_invert_rotation( CN<decltype(m_invert_rotation)> val) { m_invert_rotation = val; }

Vec Phys::get_vel() const {
  auto direct = deg_to_vec(m_deg);
  return direct * m_speed;
}

void Phys::set_pos( CN<decltype(m_pos)> val) {
  m_pos = val;
  m_old_pos = val;
}

void Phys::set_deg( CN<decltype(m_deg)> val) { m_deg = ring_deg(val); }
void Phys::set_rot_spd( CN<decltype(m_rotate_speed)> val) { m_rotate_speed = val; }

void Phys::set_accel( CN<decltype(m_accel)> val) {
  assert(val >= 0);
  m_accel = val;
}

void Phys::set_rot_ac( CN<decltype(m_rotate_accel)> val) {
  assert(val >= 0);
  m_rotate_accel = val;
}

void Phys::set_rot_fc( CN<decltype(m_rotate_force)> val) { m_rotate_force = std::max<decltype(m_rotate_force)>(0, val); }

void Phys::set_speed( CN<decltype(m_speed)> val) {
  m_speed = val;

  // развернуться, если скорость отрицательная
  if (m_speed < 0) {
    m_speed *= -1;
    set_deg(get_deg() + 180);
  }
}

void Phys::set_vel(const Vec val) {
  auto speed = length(val);
  // если вектор движения 0, то не менять угол разворота
  auto deg = val.not_zero() ? vec_to_deg(val) : get_deg();

  set_speed(speed);
  set_deg(deg);
}

void Phys::update(const Delta_time dt) {
  cauto fdt = dt;
  m_old_pos = get_pos();

  // применение вращения
  m_rotate_speed += m_rotate_accel * fdt * 0.5f;
  m_rotate_speed = std::max<real>(0.0f, m_rotate_speed - m_rotate_force * fdt * 0.5f);
  set_deg(m_deg + m_rotate_speed * fdt * (m_invert_rotation ? -1.0f : 1.0f));

  auto vel = get_vel();
  auto direct = get_direction();
  auto _force = get_force();

  // движение с ускорением корректным для dt
  vel += direct * m_accel * fdt * 0.5f;
  set_vel(vel);

  auto _speed = get_speed();
  _speed -= _force * fdt * 0.5f;
  set_speed(std::max<real>(0.0f, _speed));

  set_pos( get_pos() + (vel * fdt) );

  // докручиваем чтоб было ровно по интегралу
  m_rotate_speed += m_rotate_accel * fdt * 0.5f;
  m_rotate_speed = std::max<real>(0.0f, m_rotate_speed - m_rotate_force * fdt * 0.5f);
  set_deg(m_deg + m_rotate_speed * fdt * (m_invert_rotation ? -1.0f : 1.0f));

  vel = get_vel();
  direct = get_direction();
  vel += direct * m_accel * fdt * 0.5f;
  set_vel(vel);

  _speed = get_speed();
  _speed -= _force * fdt * 0.5f;
  set_speed(std::max<real>(0.0f, _speed));

  assert(get_deg() >= 0.0f && get_deg() <= 360);
  set_vel(vel);
} // update
