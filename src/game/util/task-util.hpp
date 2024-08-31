#pragma once
#include <functional>
#include "task.hpp"

// выполняет задачу по прошествии указанного времени
class Task_timed final: public Task {
  struct Impl;
  Unique<Impl> _impl {};

public:
  using Action = std::function<void ()>;

  explicit Task_timed(const Delta_time delay, cr<Action> action);
  ~Task_timed();
  void update(const Delta_time dt) override;
};
