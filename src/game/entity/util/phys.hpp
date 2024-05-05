#pragma once
#include "util/math/vec.hpp"

// Physical context
class Phys final {
  Vec m_old_pos {}; // старая позиция
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

  CN<decltype(m_old_pos)> get_old_pos() const;
  CN<decltype(m_pos)> get_pos() const;
  Vec get_vel() const;
  Vec get_direction() const;
  CN<decltype(m_deg)> get_deg() const;
  CN<decltype(m_accel)> get_accel() const;
  CN<decltype(m_force)> get_force() const;
  CN<decltype(m_speed)> get_speed() const;
  CN<decltype(m_rotate_speed)> get_rot_spd() const; // get rotation m_speed
  CN<decltype(m_rotate_accel)> get_rot_ac() const; // get rotation m_accel
  CN<decltype(m_rotate_force)> get_rot_fc() const; // get rotation m_force
  CN<decltype(m_invert_rotation)> get_invert_rotation() const;
  
  void set_pos( CN<decltype(m_pos)> val);
  void set_vel( const Vec val);
  void set_deg( CN<decltype(m_deg)> val);
  void set_accel( CN<decltype(m_accel)> val);
  void set_force( CN<decltype(m_force)> val);
  void set_speed( CN<decltype(m_speed)> val);
  void set_rot_spd( CN<decltype(m_rotate_speed)> val); // set rotation m_speed
  void set_rot_ac( CN<decltype(m_rotate_accel)> val); // set rotation m_accel
  void set_rot_fc( CN<decltype(m_rotate_force)> val); // set rotation m_force
  void set_invert_rotation( CN<decltype(m_invert_rotation)> val);

  void update(double dt);
}; // Phys
