#pragma once
#include "util/mem-types.hpp"
#include "util/str.hpp"
#include "util/vector-types.hpp"
#include "util/macro.hpp"

namespace epge {

// описание параметра для настроек плагина
struct Param {
  Str title {}; // отображаемое название опции
  Str desc {}; // коммент с пояснением параметра

  virtual void set_value(cr<Str> val) = 0;
  virtual Str get_value() const = 0;
  virtual inline void set_max(cr<Str> val) noexcept {} // максимальный предел параметра
  virtual inline void set_min(cr<Str> val) noexcept {} // минимальный предел параметра
  virtual inline Str get_max() const noexcept { return {}; }
  virtual inline Str get_min() const noexcept { return {}; }
  virtual inline void set_step(cr<Str> step) {} // шаг с которым меняется параметр
  virtual inline Str get_step() const noexcept { return {}; }
  virtual inline void set_fast_step(cr<Str> step) {} // ускоренное изменение параметра
  virtual inline Str get_fast_step() const noexcept { return {}; }
  virtual void plus_value() = 0;
  virtual void minus_value() = 0;
  virtual inline void plus_value_fast() noexcept {}
  virtual inline void minus_value_fast() noexcept {}
  virtual inline void enable() { plus_value(); }
}; // Param

using Params = Vector< Shared<Param> >;

} // epge ns
