#pragma once
#include "collider.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/image-fwd.hpp"

class Collider_experimental_2 final: public Collider {
  nocopy(Collider_experimental_2);
  struct Impl;
  Unique<Impl> impl {};

public:
  Collider_experimental_2();
  ~Collider_experimental_2();
  void operator()(cr<Entities> entities, Delta_time dt) override;
  void debug_draw(Image& dst, const Vec camera_offset) override;
};
