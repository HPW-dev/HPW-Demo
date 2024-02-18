#include <cassert>
#include <cmath>
#include <utility>
#include "rotation.hpp"
#include "graphic-util.hpp"
#include "graphic/image/image.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/util/util-templ.hpp"
#include "util/math/mat.hpp"

Image mirror_h(CN<Image> src) {
  assert(src);
  Image ret(src.X, src.Y);
  cfor (y, ret.Y)
  cfor (x, ret.X)
    ret.fast_set(x, y, src(x, ret.Y - y - 1), {});
  return ret;
}

Image mirror_v(CN<Image> src) {
  assert(src);
  Image ret(src.X, src.Y);
  cfor (y, ret.Y)
  cfor (x, ret.X)
    ret.fast_set(x, y, src(ret.X - x - 1, y), {});
  return ret;
}

Sprite mirror_h(CN<Sprite> src) {
  assert(src);
  Sprite ret;
  ret.move_image(std::move( mirror_h(*src.get_image()) ));
  ret.move_mask(std::move( mirror_h(*src.get_mask()) ));
  return ret;
}

Sprite mirror_v(CN<Sprite> src) {
  assert(src);
  Sprite ret;
  ret.move_image(std::move( mirror_v(*src.get_image()) ));
  ret.move_mask(std::move( mirror_v(*src.get_mask()) ));
  return ret;
}

Image rotate90(CN<Image> src, uint pass) {
  assert(src);
  if (pass == 0)
    return Image(src);
  Image dst{src.Y, src.X};
  cfor(y, dst.Y)
  cfor(x, dst.X)
    dst(x, y) = src(y, dst.X - x - 1);
  if (pass > 1) return rotate90(dst, pass - 1);
  return dst;
}

Sprite rotate90(CN<Sprite> src, uint pass) {
  Sprite dst;
  dst.move_image(std::move( rotate90(*src.get_image(), pass) ));
  dst.move_mask(std::move( rotate90(*src.get_mask(), pass) ));
  return dst;
}

void rotate(CN<Image> src, Image& dst, const Vec center,
const Vec offset, real degree) {
  assert(src);
  assert(dst);
  if (degree == 0) {
    insert(dst, src, offset - center);
    return;
  }
  
  auto radian {deg_to_rad(-degree)};
  real mul_x {std::cos(radian)};
  real mul_y {std::sin(radian)};
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    auto fx {x - offset.x};
    auto fy {y - offset.y};
    auto rot_x {(mul_x * fx) - (mul_y * fy) + center.x};
    int i_x = std::floor(rot_x);
    auto rot_y {(mul_y * fx) + (mul_x * fy) + center.y};
    int i_y = std::floor(rot_y);
    if (src.index_bound(i_x, i_y, Image_get::NONE)) {
      auto pix = src(i_x, i_y);
      dst.set(x, y, pix);
    }
  } // for y, x
} // rotate (image)

void rotate(CN<Sprite> src, Sprite &dst, const Vec center,
const Vec offset, real degree) {
  rotate(*src.get_image(), *dst.get_image(), center, offset, degree);
  rotate(*src.get_mask(), *dst.get_mask(), center, offset, degree);
}
