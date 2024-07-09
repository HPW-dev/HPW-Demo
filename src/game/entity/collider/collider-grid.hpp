#pragma once
#include "collider.hpp"

class Image;
struct Vec;

// разбиение пространства сеткой
class Collider_grid final: public Collider {
  nocopy(Collider_grid);
  struct Impl;
  Unique<Impl> impl {};

public:
  Collider_grid();
  ~Collider_grid();
  void operator()(CN<Entities> entities, Delta_time dt) override;
  void debug_draw(Image& dst, const Vec camera_offset) override;
};
