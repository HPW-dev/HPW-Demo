#pragma once
#include "util/macro.hpp"
#include "util/math/num-types.hpp"

template <class T>
concept have_xy = requires { T::x; T::y; };

/// 2D вектор
struct Vec {
  real x {};
  real y {};

  Vec() = default;
  inline constexpr Vec(auto _x, auto _y) noexcept: x(_x), y(_y) {}
  inline constexpr Vec(have_xy auto in) noexcept: x(in.x), y(in.y) {}
  inline constexpr Vec(CN<Vec> other) noexcept: x {other.x}, y {other.y} {}
  inline constexpr Vec(Vec&& other) noexcept: x {other.x}, y {other.y} {}
  inline constexpr Vec& operator=(CN<Vec> other) noexcept { x = other.x; y = other.y; return *this; }
  inline constexpr Vec& operator=(Vec&& other) noexcept { x = other.x; y = other.y; return *this; }

  inline constexpr bool operator ==(const Vec in) const { return x == in.x && y == in.y; }
  inline constexpr bool operator !=(const Vec in) const { return x != in.x || y != in.y; }

  inline constexpr Vec operator -() const { return Vec(x * -1, y * -1); }
  inline constexpr operator bool () const { return x != 0 || y != 0; }

  inline constexpr void operator +=(cnauto other);
  inline constexpr void operator -=(cnauto other);
  inline constexpr void operator *=(cnauto other);
  inline constexpr void operator /=(cnauto other);
}; // Vec

inline constexpr void Vec::operator +=(cnauto other) {
  if constexpr (requires { other.x; other.y; }) {
    x += other.x;
    y += other.y;
  } else {
    x += other;
    y += other;
  }
}

inline constexpr void Vec::operator -=(cnauto other) {
  if constexpr (requires { other.x; other.y; }) {
    x -= other.x;
    y -= other.y;
  } else {
    x -= other;
    y -= other;
  }
}

inline constexpr void Vec::operator *=(cnauto other) {
  if constexpr (requires { other.x; other.y; }) {
    x *= other.x;
    y *= other.y;
  } else {
    x *= other;
    y *= other;
  }
}

inline constexpr void Vec::operator /=(cnauto other) {
  if constexpr (requires { other.x; other.y; }) {
    x /= other.x;
    y /= other.y;
  } else {
    x /= other;
    y /= other;
  }
}

inline constexpr Vec operator + (const Vec a, cnauto b) {
  if constexpr (requires { b.x; b.y; }) {
    return Vec(a.x + b.x, a.y + b.y);
  } else {
    return Vec(a.x + b, a.y + b);
  }
}

inline constexpr Vec operator - (const Vec a, cnauto b) {
  if constexpr (requires { b.x; b.y; }) {
    return Vec(a.x - b.x, a.y - b.y);
  } else {
    return Vec(a.x - b, a.y - b);
  }
}

inline constexpr Vec operator * (const Vec a, cnauto b) {
  if constexpr (requires { b.x; b.y; }) {
    return Vec(a.x * b.x, a.y * b.y);
  } else {
    return Vec(a.x * b, a.y * b);
  }
}

inline constexpr Vec operator / (const Vec a, cnauto b) {
  if constexpr (requires { b.x; b.y; }) {
    return Vec(a.x / b.x, a.y / b.y);
  } else {
    return Vec(a.x / b, a.y / b);
  }
}
