#pragma once
#include "collider.hpp"

class Image;
struct Vec;

// разбиение пространства прямыми
class Collider_2d_tree final: public Collider {
  nocopy(Collider_2d_tree);
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Collider_2d_tree(uint depth=5);
  ~Collider_2d_tree();
  void operator()(CN<Entities> entities, Delta_time dt) override;
  void debug_draw(Image& dst, const Vec camera_offset) override;
};
