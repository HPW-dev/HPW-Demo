#pragma once
#include "num-types.hpp"

// счётчик обратного отсчёта
class Timer final {
  Delta_time m_timer {}; // текущее время
  Delta_time m_timer_max {};
  Delta_time m_loop_limit {1};

  void set_timer(const Delta_time new_timer);

public:
  Timer() = default;
  explicit Timer(const Delta_time new_timer, const uint new_loop_limit=30u);
  template <typename T> void operator = (T x) { set_timer(x); }
  inline void reset() { m_timer = m_timer_max; }
  // оставшееся время в (0..1)
  Delta_time ratio() const;
  // сколько раз сработал таймер использовать это для for
  unsigned update(const Delta_time dt);
  // ставит таймер на случайную позицию
  void randomize_stable();
}; // Timer
