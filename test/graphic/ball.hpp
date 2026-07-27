#pragma once
#include <utility>
#include "util/math/vec.hpp"
#include "graphic/image/color.hpp"
#include "graphic/image/image-fwd.hpp"

// ball for graphic-test
class Ball final {
public:
  Vec pos = {};
  Vec dir = {};
  Pal8 color = {};
  real speed = 0;

  explicit Ball(real speed_=0, const Pal8 color_=Pal8::red);
  void random_init();
  void move(const Delta_time dt);
  void draw(Image& dst) const;
};
