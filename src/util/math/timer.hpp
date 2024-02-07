#pragma once

/// счётчик обратного отсчёта
class Timer final {
  double m_timer {}; /// текущее время
  double m_timer_max {};
  double m_loop_limit {};

  void set_timer(double new_timer);

public:
  explicit Timer(double new_timer=0, unsigned new_loop_limit=30u);
  template <typename T> void operator = (T x) { set_timer(x); }
  inline void reset() { m_timer = m_timer_max; }
  /// оставшееся время в (0..1)
  double ratio() const;
  /// сколько раз сработал таймер использовать это для for
  unsigned update(double dt);
  /// ставит таймер на случайную позицию
  void randomize();
}; // Timer
