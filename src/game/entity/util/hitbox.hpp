#pragma once
#include "util/math/circle.hpp"
#include "util/math/polygon.hpp"

using Polygons = Vector<Polygon>;
class Image;
struct Vec;
struct Pal8;

class Hitbox final {
public:
  Circle simple {}; /// для оптимизации внешний хитбокс в виде круга
  Polygons polygons {}; /// внутренний хитбокс из полигонов

  bool is_collided_with(const Vec this_pos, const Vec other_pos, CN<Hitbox> other) const;
  void draw(const Vec this_pos, Image& dst, const Pal8 color) const;
  void rotate(real degree);
  /// make it horizontally symmetrical
  void make_h_symm();
  /// проверить что два хитбокса равны
  bool operator ==(CN<Hitbox> other) const;
  /// проверить что два хитбокса не равны
  bool operator !=(CN<Hitbox> other) const;
  /// проверить что хитбокс не пустой (всё != 0)
  operator bool() const;
};
