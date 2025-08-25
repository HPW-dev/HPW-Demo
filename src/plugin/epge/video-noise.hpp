#pragma once
#include "epge.hpp"
#include "util/mem-types.hpp"

namespace epge {

// зернистость/шум
class Video_noise: public epge::Base {
  struct Impl;
  Unique<Impl> impl {};

public:
  Video_noise();
  ~Video_noise();
  Str name() const noexcept final;
  Str desc() const noexcept final;
  void draw(Image& dst) const noexcept final;
  Params params() noexcept final;
};

} // epge ns
