#pragma once
#include "epge.hpp"
#include "util/mem-types.hpp"

namespace epge {

// меняет масштаб изображения
class Resize: public epge::Base {
  struct Impl;
  Unique<Impl> impl {};

public:
  Resize();
  ~Resize();
  Str name() const noexcept final;
  Str desc() const noexcept final;
  void draw(Image& dst) const noexcept final;
  Params params() noexcept final;
};

} // epge ns
