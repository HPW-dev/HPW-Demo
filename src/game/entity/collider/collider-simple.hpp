#pragma once
#include "collider.hpp"

class Image;
struct Vec;

// поиск столкновений через перебор всех объектов
class Collider_simple final: public Collider {
  nocopy(Collider_simple);
  struct Impl;
  Unique<Impl> impl {};

public:
  Collider_simple();
  ~Collider_simple();
  void operator()(cr<Entities> entities, Delta_time dt) override;
  void debug_draw(Image& dst, const Vec camera_offset) override;
};
