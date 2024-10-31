#pragma once
#include "util/macro.hpp"
#include "util/math/num-types.hpp"
#include "util/hpw-concept.hpp"

// 2D вектор
struct Vec {
  real x {};
  real y {};

  Vec() noexcept = default;
  inline constexpr Vec(auto _x, auto _y) noexcept: x(_x), y(_y) {}
  inline constexpr Vec(have_xy auto in) noexcept: x(in.x), y(in.y) {}
  inline constexpr Vec(cr<Vec> other) noexcept: x {other.x}, y {other.y} {}
  inline constexpr Vec(Vec&& other) noexcept: x {other.x}, y {other.y} {}
  inline constexpr Vec& operator=(cr<Vec> other) noexcept { x = other.x; y = other.y; return *this; }
  inline constexpr Vec& operator=(Vec&& other) noexcept { x = other.x; y = other.y; return *this; }

  inline constexpr bool operator ==(const Vec in) const noexcept { return x == in.x && y == in.y; }
  inline constexpr bool operator !=(const Vec in) const noexcept { return x != in.x || y != in.y; }

  inline constexpr Vec operator -() const noexcept { return Vec(x * -1, y * -1); }
  inline constexpr bool not_zero() const noexcept { return x != 0 || y != 0; }
  inline constexpr bool is_zero() const noexcept { return x == 0 && y == 0; }

  inline constexpr void operator +=(crauto other) noexcept;
  inline constexpr void operator -=(crauto other) noexcept;
  inline constexpr void operator *=(crauto other) noexcept;
  inline constexpr void operator /=(crauto other) noexcept;
  
  inline constexpr bool operator > (crauto other) const noexcept;
  inline constexpr bool operator >= (crauto other) const noexcept;
  inline constexpr bool operator < (crauto other) const noexcept;
  inline constexpr bool operator <= (crauto other) const noexcept;
}; // Vec

inline constexpr void Vec::operator +=(crauto other) noexcept {
  if constexpr (requires { other.x; other.y; }) {
    x += other.x;
    y += other.y;
  } else {
    x += other;
    y += other;
  }
}

inline constexpr void Vec::operator -=(crauto other) noexcept {
  if constexpr (requires { other.x; other.y; }) {
    x -= other.x;
    y -= other.y;
  } else {
    x -= other;
    y -= other;
  }
}

inline constexpr void Vec::operator *=(crauto other) noexcept {
  if constexpr (requires { other.x; other.y; }) {
    x *= other.x;
    y *= other.y;
  } else {
    x *= other;
    y *= other;
  }
}

inline constexpr void Vec::operator /=(crauto other) noexcept {
  if constexpr (requires { other.x; other.y; }) {
    x /= other.x;
    y /= other.y;
  } else {
    x /= other;
    y /= other;
  }
}

inline constexpr Vec operator + (const Vec a, crauto b) noexcept {
  if constexpr (requires { b.x; b.y; }) {
    return Vec(a.x + b.x, a.y + b.y);
  } else {
    return Vec(a.x + b, a.y + b);
  }
}

inline constexpr Vec operator - (const Vec a, crauto b) noexcept {
  if constexpr (requires { b.x; b.y; }) {
    return Vec(a.x - b.x, a.y - b.y);
  } else {
    return Vec(a.x - b, a.y - b);
  }
}

inline constexpr Vec operator * (const Vec a, crauto b) noexcept {
  if constexpr (requires { b.x; b.y; }) {
    return Vec(a.x * b.x, a.y * b.y);
  } else {
    return Vec(a.x * b, a.y * b);
  }
}

inline constexpr Vec operator / (const Vec a, crauto b) noexcept {
  if constexpr (requires { b.x; b.y; }) {
    return Vec(a.x / b.x, a.y / b.y);
  } else {
    return Vec(a.x / b, a.y / b);
  }
}

inline constexpr bool Vec::operator > (crauto other) const noexcept {
  if constexpr (requires { other.x; other.y; }) {
    return x > other.x && y > other.y;
  } else {
    return x > other && y > other;
  }
}
inline constexpr bool Vec::operator >= (crauto other) const noexcept {
  if constexpr (requires { other.x; other.y; }) {
    return x >= other.x && y >= other.y;
  } else {
    return x >= other && y >= other;
  }
}
inline constexpr bool Vec::operator < (crauto other) const noexcept {
  if constexpr (requires { other.x; other.y; }) {
    return x < other.x && y < other.y;
  } else {
    return x < other && y < other;
  }
}
inline constexpr bool Vec::operator <= (crauto other) const noexcept {
  if constexpr (requires { other.x; other.y; }) {
    return x <= other.x && y <= other.y;
  } else {
    return x <= other && y <= other;
  }
}
