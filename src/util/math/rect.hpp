#pragma once
#include "util/math/vec.hpp"

struct Rect {
  Vec pos {};
  Vec size {};

  Rect() = default;
  constexpr Rect(have_xy auto _pos, have_xy auto _size): pos(_pos), size(_size) {}
  constexpr Rect(auto pos_x, auto pos_y, auto size_x, auto size_y)
    : pos(pos_x, pos_y), size(size_x, size_y) {}
};

bool intersect(const Rect a, const Rect b);
