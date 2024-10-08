#pragma once
#include "vec.hpp"

// круг
struct Circle {
  Vec offset {}; // смещение при отрисовке
  real r; // radius

  // проверить пересечения двух кругов
  bool is_collided(const Vec this_pos, const Vec other_pos, cr<Circle> other) const;
  bool operator ==(cr<Circle> other) const;
  bool operator !=(cr<Circle> other) const;
  operator bool() const;
};
