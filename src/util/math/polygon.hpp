#pragma once
#include "vec.hpp"
#include "util/vector-types.hpp"

/// выпуклый многоугольник
struct Polygon {
  Vec offset {}; /// доп. смещение
  Vector<Vec> points {};

  bool collided_with(const Vec this_center, const Vec other_center, CN<Polygon> other) const;
  /// проверить что два полигона равны
  bool operator ==(CN<Polygon> other) const;
  /// проверить что два полигона не равны
  bool operator !=(CN<Polygon> other) const;
  /// проверить что полигон не пустой (всё != 0)
  operator bool() const;
};
