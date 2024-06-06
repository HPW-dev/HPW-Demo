#include <cmath>
#include <cassert>
#include <algorithm>
#include "graphic-util.hpp"
#include "rotsprite.hpp"
#include "graphic/image/image.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/util/rotation.hpp"
#include "util/log.hpp"
#include "util/math/vec-util.hpp"

Sprite rotate_and_optimize(CN<Sprite> src, real degree, Vec& offset,
const Vec rotation_offset, Color_get_pattern cgp, Color_compute ccf) {
  assert(src);
  // выйти, если ничего не поворачивается
  return_if (degree == 0, optimize_size(src, offset));
  // аккуратный поворот пиксель-арта
  static Sprite scaled; // prebuf opt
  scaled = pixel_upscale_x3(src);
  offset *= 3; // и смещение тоже апскейлится
  // чтобы повёрнутая картинка уместилась
  real max_size = std::max(scaled.X(), scaled.Y());
  max_size = std::ceil( real(max_size * 1.3) );

  static Sprite for_rotate; // prebuf opt
  for_rotate.image().assign_resize(max_size, max_size);
  for_rotate.mask().assign_resize(max_size, max_size);
  rotate(scaled, for_rotate, center_point(scaled) + rotation_offset,
    center_point(for_rotate), degree);
  // смена оффсета, с учётом картинки для поворота и вращение смещения
  offset.x -= (for_rotate.X() - scaled.X()) / 2.0;
  offset.y -= (for_rotate.Y() - scaled.Y()) / 2.0;
  offset = rotate_deg(
    -Vec(for_rotate.X() / 2.0, for_rotate.Y() / 2.0) + rotation_offset,
    offset, degree
  );
  // даунскейл смещения
  offset /= 3.0;
  return optimize_size(pixel_downscale_x3(for_rotate, cgp, ccf), offset);
} // rotate_and_optimize
