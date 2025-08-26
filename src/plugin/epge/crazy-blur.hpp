#pragma once
#include "epge.hpp"
#include "util/mem-types.hpp"

namespace epge {

// случайно растягивает части картинки
class Crazy_blur: public epge::Base {
  struct Impl;
  Unique<Impl> impl {};

public:
  Crazy_blur();
  ~Crazy_blur();
  Str name() const noexcept final;
  Str desc() const noexcept final;
  void draw(Image& dst) const noexcept final;
  Params params() noexcept final;
};

} // epge ns
