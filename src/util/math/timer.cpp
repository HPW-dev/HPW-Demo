#include <cassert>
#include "timer.hpp"
#include "util/macro.hpp"
#include "util/math/random.hpp"

void Timer::set_timer(double new_timer) {
  assert(new_timer >= 0);
  assert(new_timer < 100'000);
  m_timer_max = m_timer = new_timer;
}

Timer::Timer(double new_timer, unsigned new_loop_limit)
: m_loop_limit (new_loop_limit)
{
  assert(m_loop_limit > 0);
  set_timer(new_timer);
}

double Timer::ratio() const { return m_timer / m_timer_max; }

unsigned Timer::update(double dt) {
  assert(m_timer_max > 0);
  m_timer -= dt;
  unsigned loops = 0; // сколько раз таймер сработал за один вызов

  while (true) {
    // если время вышло, то засчитать как одно срабатывание таймера
    if (m_timer <= 0) {
      m_timer += m_timer_max; // вернуть таймер на место
      ++loops;
    } else {
      break;
    }
    // выход по превышению лимита повторений
    if (loops >= m_loop_limit) {
      loops = m_loop_limit;
      m_timer = m_timer_max;
      break;
    }
  }
  
  return loops;
}

void Timer::randomize_stable() { m_timer = rndr(0, m_timer_max); }
