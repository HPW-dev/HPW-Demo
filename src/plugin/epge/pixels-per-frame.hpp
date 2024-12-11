#pragma once
#include "epge.hpp"
#include "util/mem-types.hpp"

namespace epge {

// ресует определённое число пикселей за кадр
class Pixels_per_frame: public epge::Base {
  struct Impl;
  Unique<Impl> impl {};

public:
  Pixels_per_frame();
  ~Pixels_per_frame();
  Str name() const noexcept final;
  Str desc() const noexcept final;
  void draw(Image& dst) const noexcept final;
  Params params() noexcept final;
};

} // epge ns
