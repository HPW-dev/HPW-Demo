#pragma once
#include "collider.hpp"
#include "util/math/vec.hpp"

class Image;

// разбиение пространства сеткой
class Collider_grid final: public Collider {
  nocopy(Collider_grid);
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Collider_grid(int grid_sz=26);
  ~Collider_grid();
  void operator()(cr<Entities> entities, Delta_time dt) override;
  void debug_draw(Image& dst, const Vec camera_offset) override;
};
