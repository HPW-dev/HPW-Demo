#include <cassert>
#include <algorithm>
#include <cmath>
#include <limits>
#include "gjk.h"
#include "polygon.hpp"
#include "util/vector-types.hpp"


#ifdef CLD_DEBUG
#include "game/core/debug.hpp"
#endif

bool Polygon::collided_with(const Vec this_center, const Vec other_center,
cr<Polygon> other) const {
  // добавить смещения
	auto this_offset = offset + this_center;
  auto this_poly = *this;
  for (rauto point: this_poly.points)
    point += this_offset;

	auto other_offset = other.offset + other_center;
  auto other_poly = other;
  for (rauto point: other_poly.points)
    point += other_offset;

  #ifdef CLD_DEBUG
  ++hpw::poly_checks;
  #endif

  static_assert(sizeof(vec2) == sizeof(Vec));
  return gjk (
    cptr2ptr<cp<vec2>>(this_poly.points.data()), this_poly.points.size(),
    cptr2ptr<cp<vec2>>(other_poly.points.data()), other_poly.points.size() );
} // collided_with

bool Polygon::operator ==(cr<Polygon> other) const {
  if (this->offset != other.offset)
    return false;

  // если число точек не совпадает, сразу выход
  auto points_count = this->points.size();
  if (points_count != other.points.size())
    return false;
  // почекать все точки полигона
  cfor (p, points_count)
    if (this->points.at(p) != other.points.at(p))
      return false;
      
  return true;
} // op ==

bool Polygon::operator !=(cr<Polygon> other) const
  { return !( operator==(other) ); }

Polygon::operator bool() const { 
  if (offset.not_zero())
    return true;

  for (crauto point: points)
    if (point.not_zero())
      return true;

  return false;
}
