#include <cassert>
#include "task-util.hpp"
#include "util/math/timer.hpp"

struct Task_timed::Impl {
  Task_timed& _master;
  Timer _delay {};
  Action _action {};

  inline explicit Impl(Task_timed& master, const Delta_time delay, cr<Action> action)
  : _master(master)
  , _delay(delay)
  , _action(action)
  {
    assert(delay >= 0);
    assert(action);
  }

  inline void update(const Delta_time dt) {
    return_if (!_delay.update(dt));
    _action();
    _master.deactivate();
  }
};

Task_timed::Task_timed(const Delta_time delay, cr<Action> action)
  : _impl {new_unique<Impl>(*this, delay, action)} {}
Task_timed::~Task_timed() {}
void Task_timed::update(const Delta_time dt) { _impl->update(dt); }
