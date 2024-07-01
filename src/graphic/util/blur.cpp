#ifndef __clang__
#include <omp.h>
#endif

#include <cassert>
#include "blur.hpp"
#include "graphic/image/image.hpp"

void adaptive_blur(Image& dst, int window_sz) {
  assert(dst);
  assert(window_sz >= 1);

  Image src (dst);
  real accum_mul = 1.0 / ((window_sz * 2) * (window_sz * 2));

  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    real accum {0};

    for (int wx = -window_sz; wx < window_sz; ++wx)
    for (int wy = -window_sz; wy < window_sz; ++wy)
      accum += src.get(x + wx, y + wy, Image_get::MIRROR).to_real();

    accum *= accum_mul;
    nauto dst_pix = dst(x, y);
    dst_pix = Pal8::from_real(accum, dst_pix.is_red());
  }
} // adaptive_blur

void adaptive_blur_fat_red(Image& dst, int window_sz) {
  assert(dst);
  assert(window_sz >= 1);

  Image src (dst);
  real accum_mul = 1.0 / ((1 + window_sz * 2) * (1 + window_sz * 2));

  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    bool is_red {false};
    real accum {0};

    for (int wx = -window_sz; wx < window_sz; ++wx)
    for (int wy = -window_sz; wy < window_sz; ++wy) {
      cauto src_pix = src.get(x + wx, y + wy, Image_get::MIRROR);
      accum += src_pix.to_real();
      is_red |= src_pix.is_red();
    }

    accum *= accum_mul;
    dst(x, y) = Pal8::from_real(accum, is_red);
  }
} // adaptive_blur_fat_red

void blur_fast(Image& dst, int window_sz) {
  assert(dst);
  window_sz *= 2;
  assert(window_sz >= 1);
  assert(dst.size > window_sz);
  const real accum_mul = 1.0 / window_sz;

  cfor (i, dst.size - window_sz) {
    real accum {0};
    cfor (wi, window_sz)
      accum += dst[i + wi].to_real();
    accum *= accum_mul;

    nauto dst_pix = dst[i];
    dst_pix = Pal8::from_real(accum, dst_pix.is_red());
  }
} // blur_fast
