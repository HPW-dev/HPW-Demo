#pragma once
#include "util/math/num-types.hpp"

// Мигает флагом в зависимости от времени
class Bg_blink final {
  Delta_time m_time_start {};
  Delta_time m_time {};
  bool& m_blink_flag;

public:
  // меняет blink_flag так, чтобы он мигал в зависимости от времени time
  explicit Bg_blink(bool& blink_flag, const Delta_time time);
  bool operator()(Delta_time dt);
};
