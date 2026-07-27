#pragma once
#include "task.hpp"
#include "util/action.hpp"

// выполняет задачу по прошествии указанного времени
class Task_timed final: public Task {
  struct Impl;
  Unique<Impl> _impl {};

public:
  explicit Task_timed(const Delta_time delay, cr<Action> action);
  ~Task_timed();
  void update(const Delta_time dt) override;
};
