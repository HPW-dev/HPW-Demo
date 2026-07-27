#pragma once
#include "util/math/circle.hpp"
#include "util/math/polygon.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/image-fwd.hpp"

using Polygons = Vector<Polygon>;
struct Pal8;

class Hitbox final {
public:
  Circle simple {}; // для оптимизации внешний хитбокс в виде круга
  Polygons polygons {}; // внутренний хитбокс из полигонов

  bool is_collided_with(const Vec this_pos, const Vec other_pos, cr<Hitbox> other) const;
  void draw(const Vec this_pos, Image& dst, const Pal8 color) const;
  void rotate(real degree);
  // make it horizontally symmetrical
  void make_h_symm();
  // проверить что два хитбокса равны
  bool operator ==(cr<Hitbox> other) const;
  // проверить что два хитбокса не равны
  bool operator !=(cr<Hitbox> other) const;
  // проверить что хитбокс не пустой (всё != 0)
  operator bool() const;
};
