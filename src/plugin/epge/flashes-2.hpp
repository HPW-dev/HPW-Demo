#pragma once
#include "epge.hpp"
#include "util/mem-types.hpp"

namespace epge {

// рисует вспышки на ярких пикселях
class Flashes_2: public epge::Base {
  struct Impl;
  Unique<Impl> impl {};

public:
  Flashes_2();
  ~Flashes_2();
  Str name() const noexcept final;
  Str desc() const noexcept final;
  void draw(Image& dst) const noexcept final;
  Params params() noexcept final;
};

} // epge ns
