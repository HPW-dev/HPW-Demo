#pragma once
#include "util/math/vec.hpp"

template <typename T>
struct Rect_base {
  using Vec_t = const Vec_base<T>;
  Vec_base<T> pos {};
  Vec_base<T> size {};

  Rect_base() noexcept = default;
  inline constexpr Rect_base(have_xy auto _pos, have_xy auto _size) noexcept: pos(_pos), size(_size) {}
  inline constexpr Rect_base(auto pos_x, auto pos_y, auto size_x, auto size_y) noexcept
    : pos(pos_x, pos_y), size(size_x, size_y) {}
};

template <typename T1, typename T2>
inline constexpr bool intersect(const Rect_base<T1> a, const Rect_base<T2> b) noexcept {
  cauto x1 = a.pos.x;
  cauto y1 = a.pos.y;
  cauto x2 = b.pos.x;
  cauto y2 = b.pos.y;
  cauto w1 = a.size.x;
  cauto h1 = a.size.y;
  cauto w2 = b.size.x;
  cauto h2 = b.size.y;

  return (x1  + w1 >= x2) && // r1 right edge past r2 left
         (x1 <= x2  + w2) && // r1 left edge past r2 right
         (y1  + h1 >= y2) && // r1 top edge past r2 bottom
         (y1 <= y2  + h2);   // r1 bottom edge past r2 top
}

using Rect = Rect_base<real>;
using Rectd = Rect_base<double>;
using Recti = Rect_base<int>;
using Recti64 = Rect_base<i64_t>;
using Rectu = Rect_base<uint>;
