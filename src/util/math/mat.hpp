#pragma once
#include <limits>
#include <numbers>
#include "util/math/num-types.hpp"

constexpr real PI = std::numbers::pi;
constexpr auto real_inf = std::numeric_limits<real>::infinity();

/// радианы в градусы
real rad_to_deg(real rad);

/// градусы в радианы
real deg_to_rad(real deg);

real ring_rad(real rad);
real ring_deg(real deg);

/// получить знак чила в виде -1, 1
real get_sign(real val);

/// безопасное деление
auto safe_div(auto a, auto b) -> decltype(a / b)
  { return (b == 0 ? 0 : a / b); }

constexpr auto pow2(auto x) { return x * x; }
