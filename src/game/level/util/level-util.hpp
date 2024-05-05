#pragma once

// Мигает флагом в зависимости от времени
class Bg_blink final {
  double m_time_start {};
  double m_time {};
  bool& m_blink_flag;

public:
  // меняет blink_flag так, чтобы он мигал в зависимости от времени time
  explicit Bg_blink(bool& blink_flag, double time);
  bool operator()(double dt);
};
