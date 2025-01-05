#pragma once
#include "epge.hpp"
#include "util/mem-types.hpp"

namespace epge {

// меняет масштаб изображения
class Color_rotation: public epge::Base {
  struct Impl;
  Unique<Impl> impl {};

public:
  Color_rotation();
  ~Color_rotation();
  Str name() const noexcept final;
  Str desc() const noexcept final;
  void update(const Delta_time dt) noexcept final;
  void draw(Image& dst) const noexcept final;
  Params params() noexcept final;
};

} // epge ns
