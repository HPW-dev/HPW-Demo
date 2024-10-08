#pragma once
#include <cassert>
#include "level-tasks.hpp"
#include "util/math/timer.hpp"

// задача с таймаутом
struct Timed_task {
  Level_task m_task {};
  Timer m_timer {};

  inline explicit Timed_task(const Delta_time timer, cr<Level_task> task={})
  : m_task {task}
  , m_timer {timer}
  { assert(timer > 0); }

  inline bool operator()(Delta_time dt) {
    if (m_task)
      if (m_task(dt))
        return true;
    return m_timer.update(dt);
  }
};
