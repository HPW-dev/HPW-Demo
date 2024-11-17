#pragma once
#include "epge.hpp"
#include "util/mem-types.hpp"

// эффект пиксельного дисплея 
class Epge_crt: public epge::Base {
  struct Impl;
  Unique<Impl> impl {};

public:
  Epge_crt();
  ~Epge_crt();
  Str name() const noexcept final;
  Str desc() const noexcept final;
  void draw(Image& dst) const noexcept final;
  void update(const Delta_time dt) noexcept final;
  epge::Params params() noexcept final;
};
