#include <omp.h>
#include <array>
#include <cmath>
#include <cassert>
#include <algorithm>
#include "bg-pattern-3.hpp"
#include "graphic/image/image.hpp"
#include "graphic/effect/dither.hpp"
#include "graphic/util/blend.hpp"
#include "graphic/util/blur.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/math/xorshift.hpp"

void bgp_liquid(Image& dst, const int bg_state) {
  cauto state = bg_state * 2;

  // сделать пятна
  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    real color =
      std::sin((x - y - state) / 140.f) / 2.f -
      std::cos((y +     state) / 100.f)     +
      std::cos((x + y + state) /  80.f) / 2.f -
      std::sin((y +     state) / 120.f);
    color += 1.f;
    color /= 2.f;
    color = color / (1.f + std::abs(color));
    dst(x, y) = Pal8::from_real(color);
  }

  fast_dither_bayer16x16_4bit(dst);
  to_red(dst);
}

void bgp_liquid_gray(Image& dst, const int bg_state) {
  auto state = bg_state * 2;

  // сделать пятна
  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    real color =
      std::sin((x - y - state) / 140.f) / 2.f -
      std::cos((y +     state) / 100.f)     +
      std::cos((x + y + state) /  80.f) / 2.f -
      std::sin((y +     state) / 120.f);
    color += 1.f;
    color /= 2.f;
    color = color / (1.f + std::abs(color));
    dst(x, y) = Pal8::from_real(color);
  }

  Image blured(dst);
  apply_invert(blured);

  // маска для блюра
  Image alpha_mask(dst.X, dst.Y);
  state *= 1.5;
  #pragma omp parallel for simd collapse(2)
  cfor (y, alpha_mask.Y)
  cfor (x, alpha_mask.X) {
    real color =
      std::sin((x + y + state) / 100.f) / 2.f +
      std::cos((y +     state) / 150.f)     -
      std::cos((x - y + state) /  50.f) / 3.f +
      std::sin((y +     state) / 120.f);
    color *= 3.f;
    color = color / (1.f + std::abs(color));
    alpha_mask(x, y) = Pal8::from_real(color);
  }

  accept_blend_mask_fast(dst, blured, alpha_mask);
}