#include <omp.h>
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
    rauto dst_pix = dst(x, y);
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

    rauto dst_pix = dst[i];
    dst_pix = Pal8::from_real(accum, dst_pix.is_red());
  }
} // blur_fast

void boxblur_gray_accurate(Image& dst, cr<Image> src, const int window_sz) {
  assert(dst);
  assert(src);
  assert(std::addressof(dst) != std::addressof(src));
  assert(dst.size == src.size);
  assert(window_sz >= 1);

  const int KERNEL_LEN = window_sz * 2 + 1;
  const int KERNEL_SZ = KERNEL_LEN * KERNEL_LEN;
  const real MUL = 1.f / KERNEL_SZ;

  #pragma omp parallel for simd collapse(2)
  cfor (y, src.Y)
  cfor (x, src.X) {
    real sum {};
    for (int wy = -window_sz; wy < window_sz; ++wy)
    for (int wx = -window_sz; wx < window_sz; ++wx) {
      cauto wcolor = src.get(x + wx, y + wy, Image_get::MIRROR);
      const bool is_red = wcolor.is_red();
      real luma = wcolor.to_real();
      luma = is_red ? luma / 3.f : luma;
      sum += luma * MUL;
    }
    dst(x, y) = Pal8::from_real(sum);
  }
}
void boxblur_gray_fast(Image& dst, cr<Image> src, const int window_sz) {
  assert(dst);
  assert(src);
  assert(std::addressof(dst) != std::addressof(src));
  assert(dst.size == src.size);
  assert(window_sz >= 1);

  const int KERNEL_LEN = window_sz * 2 + 1;
  assert(dst.X > KERNEL_LEN);
  assert(dst.Y > KERNEL_LEN);
  const int KERNEL_SZ = KERNEL_LEN * KERNEL_LEN;

  #pragma omp parallel for simd collapse(2)
  for (int y = window_sz; y < src.Y - window_sz; ++y)
  for (int x = window_sz; x < src.X - window_sz; ++x) {
    int sum {};
    for (int wy = -window_sz; wy < window_sz; ++wy)
    for (int wx = -window_sz; wx < window_sz; ++wx)
      sum += src(x + wx, y + wy).val;
    dst(x, y) = sum / KERNEL_SZ;
  }

  // добить края изображения растягиванием
  // vertical:
  for (int y = 0; y < window_sz; ++y)
  for (int x = 0; x < src.X; ++x)
    dst(x, y) = dst(x, window_sz);
  for (int y = src.Y - window_sz; y < src.Y; ++y)
  for (int x = 0; x < src.X; ++x)
    dst(x, y) = dst(x, src.Y - window_sz - 1);
  // horizontal:
  for (int y = 0; y < src.Y; ++y)
  for (int x = 0; x < window_sz; ++x)
    dst(x, y) = dst(window_sz, y);
  for (int y = 0; y < src.Y; ++y)
  for (int x = src.X - window_sz; x < src.X; ++x)
    dst(x, y) = dst(src.X - window_sz - 1, y);
}
