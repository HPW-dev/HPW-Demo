#pragma once
#include "collider.hpp"

// Никакой проверки столкновений
class Collider_empty final: public Collider {
  nocopy(Collider_empty);

public:
  Collider_empty() = default;
  ~Collider_empty() = default;
  inline void operator()(cr<Entities> entities, Delta_time dt) override {}
};
