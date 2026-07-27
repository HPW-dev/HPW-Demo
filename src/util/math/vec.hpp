#pragma once
#include "util/macro.hpp"
#include "util/math/num-types.hpp"
#include "util/hpw-concept.hpp"

// 2D вектор
template <typename T>
struct Vec_base {
  T x {};
  T y {};

  Vec_base() noexcept = default;
  inline constexpr Vec_base(auto _x, auto _y) noexcept: x(_x), y(_y) {}
  inline constexpr Vec_base(have_xy auto in) noexcept: x(in.x), y(in.y) {}
  inline constexpr Vec_base(cr<Vec_base> other) noexcept: x {other.x}, y {other.y} {}
  inline constexpr Vec_base(Vec_base&& other) noexcept: x {other.x}, y {other.y} {}
  inline constexpr Vec_base& operator=(cr<Vec_base> other) noexcept { x = other.x; y = other.y; return *this; }
  inline constexpr Vec_base& operator=(Vec_base&& other) noexcept { x = other.x; y = other.y; return *this; }

  inline constexpr bool operator ==(const Vec_base in) const noexcept { return x == in.x && y == in.y; }
  inline constexpr bool operator !=(const Vec_base in) const noexcept { return x != in.x || y != in.y; }

  inline constexpr Vec_base operator -() const noexcept { return Vec_base(x * -1, y * -1); }
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
}; // Vec_base

template <typename T>
inline constexpr void Vec_base<T>::operator +=(crauto other) noexcept {
  if constexpr (requires { other.x; other.y; }) {
    x += other.x;
    y += other.y;
  } else {
    x += other;
    y += other;
  }
}

template <typename T>
inline constexpr void Vec_base<T>::operator -=(crauto other) noexcept {
  if constexpr (requires { other.x; other.y; }) {
    x -= other.x;
    y -= other.y;
  } else {
    x -= other;
    y -= other;
  }
}

template <typename T>
inline constexpr void Vec_base<T>::operator *=(crauto other) noexcept {
  if constexpr (requires { other.x; other.y; }) {
    x *= other.x;
    y *= other.y;
  } else {
    x *= other;
    y *= other;
  }
}

template <typename T>
inline constexpr void Vec_base<T>::operator /=(crauto other) noexcept {
  if constexpr (requires { other.x; other.y; }) {
    x /= other.x;
    y /= other.y;
  } else {
    x /= other;
    y /= other;
  }
}

template <typename T>
inline constexpr Vec_base<T> operator + (const Vec_base<T> a, crauto b) noexcept {
  if constexpr (requires { b.x; b.y; }) {
    return Vec_base<T>(a.x + b.x, a.y + b.y);
  } else {
    return Vec_base<T>(a.x + b, a.y + b);
  }
}

template <typename T>
inline constexpr Vec_base<T> operator - (const Vec_base<T> a, crauto b) noexcept {
  if constexpr (requires { b.x; b.y; }) {
    return Vec_base<T>(a.x - b.x, a.y - b.y);
  } else {
    return Vec_base<T>(a.x - b, a.y - b);
  }
}

template <typename T>
inline constexpr Vec_base<T> operator * (const Vec_base<T> a, crauto b) noexcept {
  if constexpr (requires { b.x; b.y; }) {
    return Vec_base<T>(a.x * b.x, a.y * b.y);
  } else {
    return Vec_base<T>(a.x * b, a.y * b);
  }
}

template <typename T>
inline constexpr Vec_base<T> operator / (const Vec_base<T> a, crauto b) noexcept {
  if constexpr (requires { b.x; b.y; }) {
    return Vec_base<T>(a.x / b.x, a.y / b.y);
  } else {
    return Vec_base<T>(a.x / b, a.y / b);
  }
}

template <typename T>
inline constexpr bool Vec_base<T>::operator > (crauto other) const noexcept {
  if constexpr (requires { other.x; other.y; }) {
    return x > other.x && y > other.y;
  } else {
    return x > other && y > other;
  }
}

template <typename T>
inline constexpr bool Vec_base<T>::operator >= (crauto other) const noexcept {
  if constexpr (requires { other.x; other.y; }) {
    return x >= other.x && y >= other.y;
  } else {
    return x >= other && y >= other;
  }
}

template <typename T>
inline constexpr bool Vec_base<T>::operator < (crauto other) const noexcept {
  if constexpr (requires { other.x; other.y; }) {
    return x < other.x && y < other.y;
  } else {
    return x < other && y < other;
  }
}

template <typename T>
inline constexpr bool Vec_base<T>::operator <= (crauto other) const noexcept {
  if constexpr (requires { other.x; other.y; }) {
    return x <= other.x && y <= other.y;
  } else {
    return x <= other && y <= other;
  }
}

using Vec = Vec_base<real>;
using Vecd = Vec_base<double>;
using Veci = Vec_base<int>;
using Vecu = Vec_base<uint>;
using Veci64 = Vec_base<i64_t>;
