#pragma once
#include "collider.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/image-fwd.hpp"

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
