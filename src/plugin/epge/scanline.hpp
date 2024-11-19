#pragma once
#include "epge.hpp"
#include "util/mem-types.hpp"

namespace epge {

// эффект полосок CRT экранов
class Scanline: public epge::Base {
  struct Impl;
  Unique<Impl> impl {};

public:
  Scanline();
  ~Scanline();
  Str name() const noexcept final;
  Str desc() const noexcept final;
  void draw(Image& dst) const noexcept final;
  Params params() noexcept final;
};

} // epge ns
