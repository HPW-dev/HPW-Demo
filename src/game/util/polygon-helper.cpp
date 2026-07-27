#include <algorithm>
#include "polygon-helper.hpp"
#include "util/macro.hpp"
#include "util/math/polygon.hpp"
#include "util/math/circle.hpp"
#include "util/math/vec-util.hpp"

Circle cover_polygons(cr<Vector<Polygon>> polygons) {
  return_if (polygons.empty(), {});

  // найти крайние точки
  Vec point_min(99999, 99999);
  Vec point_max(-1, -1);
  for (crauto poly: polygons)
  for (crauto point: poly.points) {
    point_min.x = std::min(point_min.x, point.x + poly.offset.x);
    point_min.y = std::min(point_min.y, point.y + poly.offset.y);
    point_max.x = std::max(point_max.x, point.x + poly.offset.x);
    point_max.y = std::max(point_max.y, point.y + poly.offset.y);
  }

  // определить центр
  Vec mid = point_min + (point_max - point_min) * 0.5;
  // найти самую дальнюю точку от центра
  real dist = 0;
  for (crauto poly: polygons)
  for (crauto point: poly.points)
    dist = std::max(dist, distance(mid, point + poly.offset));

  return Circle(mid, dist);
}
