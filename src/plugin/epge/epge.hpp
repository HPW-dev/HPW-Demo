#pragma once
#include "epge-params.hpp"
#include "util/math/num-types.hpp"

class Image;

namespace epge {

// интерфейс для встроенных плагинов
class Base {
public:
  Base() = default;
  virtual ~Base() = default;
  virtual Str name() const = 0;
  virtual inline Str desc() const noexcept { return {}; } // коммент с описанием плагина
  virtual inline void draw(Image& dst) const noexcept {}
  virtual inline void update(const Delta_time dt) noexcept {}
  // узнать какие параметры есть у плагина
  virtual inline Params params() const noexcept { return {}; }
};

} // epge ns
