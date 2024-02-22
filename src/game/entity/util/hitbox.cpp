#include <cmath>
#include <algorithm>
#include "hitbox.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/util/sync.hpp"
#include "game/core/debug.hpp"
#include "util/math/vec.hpp"
#include "util/math/vec-util.hpp"

bool Hitbox::is_collided_with(const Vec this_pos, const Vec other_pos,
CN<Hitbox> other) const {
  // если внешние упрощённые хитбоксы столкнулись
  if (simple.is_collided(this_pos, other_pos, other.simple)) {
    // то проверить все внутренние полигоны до первого столкновения
    for (cnauto self_poly: polygons)
      for (cnauto other_poly: other.polygons)
        if (self_poly.collided_with(this_pos, other_pos, other_poly)) {
          #ifdef CLD_DEBUG
          ++hpw::total_collided;
          #endif
          return true;
        }
  } // if collided
  return false;
} // is_collided_with

void Hitbox::draw(const Vec this_pos, Image& dst, const Pal8 color) const {
  if (graphic::draw_full_hitboxes) {
    // нарисовать покрывающий упрощённый круг
    auto circle_pos = ceil(this_pos + simple.offset);
    draw_circle(dst, circle_pos, std::ceil(simple.r), color);

    if ((graphic::frame_count & 0b11) == 0) { // мигание
      // линию с крестиком до оффсета круга
      draw_line(dst, ceil(this_pos), circle_pos, color);
      draw_cross(dst, ceil(circle_pos), color);
    }
  }

  // нарисовать все полигоны хитбокса
  for (cnauto poly: polygons) {
    auto poly_pos = ceil(this_pos + poly.offset);
    draw_polygon(dst, ceil(this_pos), poly, color);

    if ((graphic::frame_count & 0b11) == 0) { // мигание
      // линию с крестиком до оффсета полигона
      draw_line(dst, ceil(this_pos), poly_pos, color);
      draw_cross(dst, ceil(poly_pos), color);
    }
  } // for polygones
} // draw

void Hitbox::rotate(real degree) {
  simple.offset = rotate_deg({}, simple.offset, degree);
  for (nauto polygon: polygons) {
    polygon.offset = rotate_deg({}, polygon.offset, degree);
    for (nauto point: polygon.points) {
      point = rotate_deg({}, point, degree);
    }
  }
}

bool Hitbox::operator ==(CN<Hitbox> other) const {
  // если число полигонов не совпадает, сразу выход
  auto polygons_count = this->polygons.size();
  if (polygons_count != other.polygons.size())
    return false;

  // почекать все полигоны
  cfor (i, polygons_count) {
    cnauto this_poly = this->polygons.at(i);
    cnauto other_poly = other.polygons.at(i);
    if (this_poly != other_poly)
      return false;
  } // for polygons_count

  return true;
} // op ==

bool Hitbox::operator !=(CN<Hitbox> other) const
  { return !( operator==(other) ); }

Hitbox::operator bool() const {
  for (cnauto poly: polygons)
    if (poly)
      return true;
  return false;
} // op bool

void Hitbox::make_h_symm() {
  // отразить все полигоны, какие есть сейчас
  auto cur_poly_count = polygons.size();
  cfor (i, cur_poly_count) {
    auto copy = polygons[i];
    copy.offset.x *= -1;
    for (nauto point: copy.points)
      point.x *= -1;
    polygons.emplace_back(copy);
  }

  /* чтобы удалить дупликаты, если эта
  функция вызывалась несколько раз */
  auto end = polygons.end();
  for (auto it = polygons.begin(); it != end; ++it)
    end = std::remove(it + 1, end, *it);

  polygons.erase(end, polygons.end());
} // make_h_symm
