#pragma once
#include "collider.hpp"

class Entity;

// Простая проверка столкновения всех со всеми
class Collider_simple final: public Collider {
  nocopy(Collider_simple);
  void test_collide(Entity& a, Entity& b);

public:
  Collider_simple() = default;
  ~Collider_simple() = default;
  void operator()(CN<Entitys> entities, double dt) override;
}; // Collider_simple
