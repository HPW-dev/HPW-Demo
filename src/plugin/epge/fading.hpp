#pragma once
#include "epge.hpp"
#include "util/mem-types.hpp"

namespace epge {

// межкадровое размытие
class Fading: public epge::Base {
  struct Impl;
  Unique<Impl> impl {};

public:
  Fading();
  ~Fading();
  Str name() const noexcept final;
  Str desc() const noexcept final;
  void draw(Image& dst) const noexcept final;
  Params params() noexcept final;
};

} // epge ns
