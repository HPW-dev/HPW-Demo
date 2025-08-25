#pragma once
#include "collider.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/image-fwd.hpp"

/* разбивает пространство расшияющимися прямоугольниками
и проверяет столькновения в них */
class Collider_experimental final: public Collider {
  nocopy(Collider_experimental);
  struct Impl;
  Unique<Impl> impl {};

public:
  Collider_experimental();
  ~Collider_experimental();
  void operator()(cr<Entities> entities, Delta_time dt) override;
  void debug_draw(Image& dst, const Vec camera_offset) override;
};
