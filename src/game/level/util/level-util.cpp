#include "level-util.hpp"
#include <cassert>
#include "util/math/random.hpp"

// меняет blink_flag так, чтобы он мигал в зависимости от времени time
Bg_blink::Bg_blink(bool& blink_flag, const Delta_time time)
: m_time_start {time}
, m_time {time}
, m_blink_flag {blink_flag}
{
  assert(m_time_start > 0);
}

bool Bg_blink::operator()(Delta_time dt) {
  auto ratio = m_time / m_time_start;
  m_blink_flag = rndr_fast() > ratio;
  m_time -= dt;
  return m_time <= 0;
}
