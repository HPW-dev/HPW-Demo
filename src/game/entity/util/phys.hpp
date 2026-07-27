#pragma once
#include "util/math/vec.hpp"

// Physical context
class Phys final {
  mutable Vec m_old_vel {-9999, 9999}; // для кеширования
  mutable Vec m_cached_dir {-999, 999}; // для кеширования
  mutable Vec m_cached_vel {-999, 999}; // для кеширования
  mutable real m_old_deg {-9999}; // для кеширования
  mutable real m_old_speed {-9999}; // для кеширования

  Vec m_pos {}; // позиция
  real m_speed {}; // скорость движения (pps)
  real m_accel {}; // ускорение (pps)
  real m_force {}; // торможение (работает как обратный m_accel)
  real m_deg {}; // degree (0..360)
  real m_rotate_speed {}; // скорость поворота (pps)
  real m_rotate_accel {}; // ускорение поворота (pps)
  real m_rotate_force {}; // замедление поворота
  bool m_invert_rotation {}; // вращаться в обратную сторону

public:
  Phys() = default;
  ~Phys() = default;

  cr<decltype(m_pos)> get_pos() const;
  Vec get_vel() const;
  Vec get_direction() const;
  cr<decltype(m_deg)> get_deg() const;
  cr<decltype(m_accel)> get_accel() const;
  cr<decltype(m_force)> get_force() const;
  cr<decltype(m_speed)> get_speed() const;
  cr<decltype(m_rotate_speed)> get_rot_spd() const; // get rotation m_speed
  cr<decltype(m_rotate_accel)> get_rot_ac() const; // get rotation m_accel
  cr<decltype(m_rotate_force)> get_rot_fc() const; // get rotation m_force
  cr<decltype(m_invert_rotation)> get_invert_rotation() const;
  
  void set_pos( cr<decltype(m_pos)> val);
  void set_vel( const Vec val);
  void set_deg( cr<decltype(m_deg)> val);
  void set_accel( cr<decltype(m_accel)> val);
  void set_force( cr<decltype(m_force)> val);
  void set_speed( cr<decltype(m_speed)> val);
  void set_rot_spd( cr<decltype(m_rotate_speed)> val); // set rotation m_speed
  void set_rot_ac( cr<decltype(m_rotate_accel)> val); // set rotation m_accel
  void set_rot_fc( cr<decltype(m_rotate_force)> val); // set rotation m_force
  void set_invert_rotation( cr<decltype(m_invert_rotation)> val);

  void update(const Delta_time dt);
}; // Phys
