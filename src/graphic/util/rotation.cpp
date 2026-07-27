#include <omp.h>
#include <cassert>
#include <cmath>
#include <utility>
#include "rotation.hpp"
#include "graphic-util.hpp"
#include "graphic/image/image.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/util/util-templ.hpp"
#include "util/math/mat.hpp"
#include "util/log.hpp"

Image mirror_h(cr<Image> src) {
  assert(src);
  Image ret(src.X, src.Y);
  cfor (y, ret.Y)
  cfor (x, ret.X)
    ret.fast_set(x, y, src(x, ret.Y - y - 1), {});
  return ret;
}

Image mirror_v(cr<Image> src) {
  assert(src);
  Image ret(src.X, src.Y);
  cfor (y, ret.Y)
  cfor (x, ret.X)
    ret.fast_set(x, y, src(ret.X - x - 1, y), {});
  return ret;
}

Sprite mirror_h(cr<Sprite> src) {
  assert(src);
  Sprite ret;
  ret.move_image( mirror_h(src.image()) );
  ret.move_mask( mirror_h(src.mask()) );
  return ret;
}

Sprite mirror_v(cr<Sprite> src) {
  assert(src);
  Sprite ret;
  ret.move_image( mirror_v(src.image()) );
  ret.move_mask( mirror_v(src.mask()) );
  return ret;
}

Image rotate_180(cr<Image> src) {
  Image dst{src.X, src.Y};
  cfor(y, dst.Y)
  cfor(x, dst.X)
    dst(x, y) = src(dst.X - x - 1, dst.Y - y - 1);
  return dst;
}

Image rotate_270(cr<Image> src) {
  Image dst{src.Y, src.X};
  cfor(y, dst.Y)
  cfor(x, dst.X)
    dst(x, y) = src(dst.Y - y - 1, x);
  return dst;
}

Image rotate_90(cr<Image> src, uint pass) {
  if (!src) {
    log_warning << "WARNING: rotate_90 src is empty";
    return {};
  }

  switch (pass % 4) {
    default:
    case 0: return src; break; // 0 deg
    case 1: break; // 90 deg
    case 2: return rotate_180(src); break; // 180 deg
    case 3: return rotate_270(src); break; // 270 deg
  }

  Image dst{src.Y, src.X};
  cfor(y, dst.Y)
  cfor(x, dst.X)
    dst(x, y) = src(y, dst.X - x - 1);
  return dst;
}

Sprite rotate_90(cr<Sprite> src, uint pass) {
  if (!src) {
    log_warning << "WARNING: rotate_90 src is empty";
    return {};
  }
  Sprite dst;
  dst.move_image( rotate_90(src.image(), pass) );
  dst.move_mask( rotate_90(src.mask(), pass) );
  return dst;
}

void rotate(cr<Image> src, Image& dst, const Vec center,
const Vec offset, real degree) {
  assert(src);
  assert(dst);
  if (degree == 0) {
    insert(dst, src, offset - center);
    return;
  }
  
  cauto radian {deg_to_rad(-degree)};
  const real mul_x {std::cos(radian)};
  const real mul_y {std::sin(radian)};
  cauto offset_x = offset.x;
  cauto offset_y = offset.y;
  cauto center_x = center.x;
  cauto center_y = center.y;

  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    cauto fx = x - offset_x;
    cauto fy = y - offset_y;
    cauto rot_x = (mul_x * fx) - (mul_y * fy) + center_x;
    cauto rot_y = (mul_y * fx) + (mul_x * fy) + center_y;

    int i_x = std::floor(rot_x);
    int i_y = std::floor(rot_y);
    if (src.index_bound(i_x, i_y, Image_get::NONE)) {
      cauto pix = src(i_x, i_y);
      dst.set(x, y, pix);
    }
  } // for y, x
} // rotate (image)

void rotate(cr<Sprite> src, Sprite &dst, const Vec center,
const Vec offset, real degree) {
  if (!src || !dst) {
    log_warning << "WARNING: rotate_90 src or dst is empty";
    return;
  }
  rotate(src.image(), dst.image(), center, offset, degree);
  rotate(src.mask(), dst.mask(), center, offset, degree);
}
