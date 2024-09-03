#pragma once
#include <functional>
#include "math/num-types.hpp"

// вызывает функцию при деструкторе и конструкторе (для областей видимости)
class Scope final {
  using Func = std::function<void ()>;
  Func m_ctor_fn {}; // функция при конструировании
  Func m_dtor_fn {}; // функция при деструкторе
  
public:
  explicit Scope(Func&& ctor_fn, Func&& dtor_fn);
  ~Scope();
};

// pixel per sec.
inline constexpr auto pps(auto in) { return in * 60; }
// pixel per sec.
inline constexpr long double operator""_pps(long double in) { return in * 60.0l; }

// x -> время ожидания в кадрах
inline constexpr Delta_time to_fps(Delta_time x) { return (1.0 / 60.0) * x; }
