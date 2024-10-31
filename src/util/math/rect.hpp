#pragma once
#include "util/math/vec.hpp"

struct Rect {
  Vec pos {};
  Vec size {};

  Rect() noexcept = default;
  inline constexpr Rect(have_xy auto _pos, have_xy auto _size) noexcept: pos(_pos), size(_size) {}
  inline constexpr Rect(auto pos_x, auto pos_y, auto size_x, auto size_y) noexcept
    : pos(pos_x, pos_y), size(size_x, size_y) {}
};

bool intersect(const Rect a, const Rect b) noexcept;
