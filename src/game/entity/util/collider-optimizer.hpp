#pragma once
#include "util/mem-types.hpp"

class Entity_mgr;

// перебирает коллайдеры при лагах для оптимальной производительности
class Collider_optimizer {
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Collider_optimizer(Entity_mgr& entity_mgr);
  ~Collider_optimizer();
  void update();
};
