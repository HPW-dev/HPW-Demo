#include <algorithm>
#include <cmath>
#include <cassert>
#include "vec-util.hpp"
#include "vec.hpp"
#include "mat.hpp"
#include "random.hpp"

#ifdef DEBUG
#include "util/str-util.hpp"
#endif

real length(const Vec src) {
  return std::sqrt(pow2(src.x) + pow2(src.y));
}

Vec normalize_stable(const Vec src) { 
  return_if (src.is_zero(), {});

  cauto len = length(src);
  assert(len != 0);
  return src / len;
}

Vec normalize_graphic(const Vec src) { 
  auto len = length(src);
  if (len == 0)
    return rand_normalized_graphic();
  return src / len;
}

Vec floor(const Vec src) { return Vec(std::floor(src.x), std::floor(src.y)); }
Vec ceil(const Vec src) { return Vec(std::ceil(src.x), std::ceil(src.y)); }
Vec abs(const Vec src) { return Vec(std::abs(src.x), std::abs(src.y)); }

real rad_between_vecs(const Vec a, const Vec b) {
  cauto dot = a.x * b.x + a.y * b.y; // dot product between [x1, y1] && [x2, y2]
  cauto det = a.x * b.y - a.y * b.x; // determinant
  return atan2(det, dot); // atan2(y, x) || atan2(sin, cos)
}

[[nodiscard]] real deg_between_vecs(const Vec a, const Vec b)
  { return rad_to_deg(rad_between_vecs(a, b)); }

Vec make_rand_by_radius_stable(const Vec pos, real r)
  { return pos + (deg_to_vec(rndr(0, 360)) * rndr(0, r)); }

Vec make_rand_by_radius_graphic(const Vec pos, real r)
  { return pos + (deg_to_vec(rndr_fast(0, 360)) * rndr_fast(0, r)); }

#ifdef DEBUG
Str to_str(const Vec src)
  { return Str("{") + n2s(src.x) + ", " + n2s(src.y) + "}"; }
#endif

Vec deg_to_vec(real deg) {
  return { std::cos(deg_to_rad(deg)), std::sin(deg_to_rad(deg)) };
}

real vec_to_deg(Vec vec) {
  if (vec.x == 0) // special cases
    return (vec.y > 0) ? 90 : (vec.y == 0) ? 0 : 270;
  else if (vec.y == 0) // special cases
    return (vec.x >= 0) ? 0 : 180;
  real ret = rad_to_deg(std::atan(vec.y / vec.x));
  if (vec.x < 0 && vec.y < 0) // quadrant Ⅲ
    ret = 180 + ret;
  else if (vec.x < 0) // quadrant Ⅱ
    ret = 180 + ret; // it actually substracts
  else if (vec.y < 0) // quadrant Ⅳ
    ret = 270 + (90 + ret); // it actually substracts
  return ret;
} // vec2deg

real rand_degree_graphic() { return rndr_fast(0, 360); }
real rand_degree_stable() { return rndr(0, 360); }

Vec rand_normalized_graphic() { return deg_to_vec(rand_degree_graphic()); }
Vec rand_normalized_stable() { return deg_to_vec(rand_degree_stable()); }

Vec rotate_rad(const Vec center, const Vec src, real radian) {
  auto cos_mul = cos(radian);
  auto sin_mul = sin(radian);
  real out_x = center.x + (src.x - center.x) * cos_mul -
    (src.y - center.y) * sin_mul;
  real out_y = center.y + (src.x - center.x) * sin_mul +
    (src.y - center.y) * cos_mul;
  return {out_x, out_y};
}

Vec rotate_deg(const Vec center, const Vec src, real degree) {
  auto radian = deg_to_rad(degree);
  auto cos_mul = cos(radian);
  auto sin_mul = sin(radian);
  real out_x = center.x + (src.x - center.x) * cos_mul -
    (src.y - center.y) * sin_mul;
  real out_y = center.y + (src.x - center.x) * sin_mul +
    (src.y - center.y) * cos_mul;
  return {out_x, out_y};
}

real distance(const Vec a, const Vec b) {
  return length(a - b);
}

real fast_distance(Vec a, const Vec b) {
  a -= b;
  return std::abs(a.x) + std::abs(a.y);
}
