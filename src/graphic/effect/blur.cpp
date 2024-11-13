#include <omp.h>
#include <cassert>
#include "blur.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/math/vec.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/mat.hpp"

void boxblur_gray_accurate(Image& dst, cr<Image> src, const int window_sz) {
  assert(dst);
  assert(src);
  assert(std::addressof(dst) != std::addressof(src));
  assert(dst.size == src.size);
  assert(window_sz >= 1);

  const int KERNEL_LEN = window_sz * 2 + 1;
  const real MUL = 1.f / (KERNEL_LEN * KERNEL_LEN);

  #pragma omp parallel for simd collapse(2)
  cfor (y, src.Y)
  cfor (x, src.X) {
    real sum {};
    for (int wy = -window_sz; wy < window_sz + 1; ++wy)
    for (int wx = -window_sz; wx < window_sz + 1; ++wx) {
      cauto wcolor = src.get(x + wx, y + wy, Image_get::MIRROR);
      const bool is_red = wcolor.is_red();
      real luma = wcolor.to_real();
      luma = is_red ? luma * 0.2989f : luma;
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
  Image src_gray(src.X, src.Y);
  to_gray_accurate(src_gray, src);

  #pragma omp parallel for simd collapse(2)
  for (int y = window_sz; y < src_gray.Y - window_sz - 1; ++y)
  for (int x = window_sz; x < src_gray.X - window_sz - 1; ++x) {
    int sum {};
    for (int wy = -window_sz; wy < window_sz + 1; ++wy)
    for (int wx = -window_sz; wx < window_sz + 1; ++wx)
      sum += src_gray(x + wx, y + wy).val;
    dst(x, y) = sum / (KERNEL_LEN * KERNEL_LEN);
  }

  // добить края изображения растягиванием
  // vertical:
  for (int y = 0; y < window_sz; ++y)
  for (int x = 0; x < src_gray.X; ++x)
    dst(x, y) = dst(x, window_sz);
  for (int y = src_gray.Y - window_sz; y < src_gray.Y; ++y)
  for (int x = 0; x < src_gray.X; ++x)
    dst(x, y) = dst(x, src_gray.Y - window_sz - 1);
  // horizontal:
  for (int y = 0; y < src_gray.Y; ++y)
  for (int x = 0; x < window_sz; ++x)
    dst(x, y) = dst(window_sz, y);
  for (int y = 0; y < src_gray.Y; ++y)
  for (int x = src_gray.X - window_sz; x < src_gray.X; ++x)
    dst(x, y) = dst(src_gray.X - window_sz - 1, y);
}

void boxblur_horizontal_gray_fast(Image& dst, cr<Image> src, const int window_sz) noexcept {
  assert(dst);
  assert(src);
  assert(std::addressof(dst) != std::addressof(src));
  assert(dst.size == src.size);
  
  Image src_gray(src.X, src.Y);
  to_gray_accurate(src_gray, src);
  boxblur_horizontal_fast(dst, src_gray, window_sz);
}

void boxblur_horizontal_fast(Image& dst, cr<Image> src, const int window_sz) noexcept {
  assert(dst);
  assert(src);
  assert(std::addressof(dst) != std::addressof(src));
  assert(dst.size == src.size);
  assert(window_sz >= 1);

  const int KERNEL_LEN = window_sz * 2 + 1;
  assert(dst.X > KERNEL_LEN);

  #pragma omp parallel for simd
  cfor (y, src.Y) {
    auto* src_ptr = &src.fast_get(window_sz, y);

    for (int x = window_sz; x < src.X - (window_sz + 1); ++x) {
      int sum {};

      cfor (wx, KERNEL_LEN)
        sum += (src_ptr + wx - window_sz)->val;
        
      dst(x, y) = sum / KERNEL_LEN;
      ++src_ptr;
    }
  }

  // добить края изображения растягиванием
  // horizontal:
  for (int y = 0; y < src.Y; ++y)
  for (int x = 0; x < window_sz; ++x)
    dst(x, y) = dst(window_sz, y);
  for (int y = 0; y < src.Y; ++y)
  for (int x = src.X - window_sz; x < src.X; ++x)
    dst(x, y) = dst(src.X - window_sz - 1, y);
}

void blur_gray_accurate(Image& dst, cr<Image> src, const int window_sz) {
  assert(dst);
  assert(src);
  assert(std::addressof(dst) != std::addressof(src));
  assert(dst.size == src.size);
  assert(window_sz >= 1);

  const int KERNEL_LEN = window_sz * 2 + 1;
  const int KERNEL_SZ = KERNEL_LEN * KERNEL_LEN;
  // создание ядра с линейным градиентом
  Vector<real> kernel(KERNEL_SZ);
  cauto MID = KERNEL_LEN / 2.f;
  const Vec CENTER (MID, MID);
  real kernel_sum {};

  cfor (y, KERNEL_LEN)
  cfor (x, KERNEL_LEN) {
    cauto len = distance(CENTER, Vec(x, y));
    cauto val = 1.f - std::clamp(len / MID, 0.f, 1.f);
    kernel_sum += val;
    kernel[y * KERNEL_LEN + x] = val;
  }

  // нормализация ядра размытия
  const real KERNEL_MUL = safe_div(1.f, kernel_sum);
  cfor (i, KERNEL_SZ)
    kernel[i] *= KERNEL_MUL;

  #pragma omp parallel for simd collapse(2)
  cfor (y, src.Y)
  cfor (x, src.X) {
    real sum {};
    for (int wy = -window_sz; wy < window_sz + 1; ++wy)
    for (int wx = -window_sz; wx < window_sz + 1; ++wx) {
      cauto wcolor = src.get(x + wx, y + wy, Image_get::MIRROR);
      const bool is_red = wcolor.is_red();
      real luma = wcolor.to_real();
      luma = is_red ? luma / 3.f : luma;
      sum += luma * kernel[(wy + window_sz) * KERNEL_LEN + (wx + window_sz)];
    }
    dst(x, y) = Pal8::from_real(sum);
  }
}
