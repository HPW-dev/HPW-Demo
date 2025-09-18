#pragma once
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"
#include "graphic/image/image-fwd.hpp"

// Для тестов графики: шарик отскакивает от экрана и издаёт звуки
class Glass_ball {
public:
  Glass_ball();
  ~Glass_ball();
  void update(Delta_time dt);
  void draw(Image& dst) const;

private:
  struct Impl;
  Unique<Impl> _impl {};
};
