#pragma once
// TODO удалить на релизе, если не понадобится
#include "collider.hpp"

// Никакой проверки столкновений
class Collider_empty final: public Collider {
  nocopy(Collider_empty);

public:
  Collider_empty() = default;
  ~Collider_empty() = default;
  inline void operator()(CN<Entitys> entities, Delta_time dt) override {}
}; // Collider_empty
