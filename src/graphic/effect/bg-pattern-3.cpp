#include <omp.h>
#include <array>
#include <cmath>
#include <cassert>
#include <algorithm>
#include "bg-pattern-3.hpp"
#include "game/core/fonts.hpp"
#include "graphic/image/image.hpp"
#include "graphic/effect/dither.hpp"
#include "graphic/util/blend.hpp"
#include "graphic/util/blur.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/math/xorshift.hpp"
#include "util/math/vec-util.hpp"

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

void bgp_trajectory(Image& dst, const int bg_state) {
  dst.fill(Pal8::red);
  assert(graphic::font);

  xorshift128_state seed {
    .a = 0x2465,
    .b = 0xAAFF,
    .c = 0xBA32,
    .d = 0x202D,
  };

  Vector<Vec> points(20);
  for (rauto point: points)
    point = Vec(
      (xorshift128(seed) & bg_state) % dst.X,
      (xorshift128(seed) & bg_state) % dst.Y
    );

  cfor (i, points.size()-3)
    draw_spline(dst, points[i], points[i+1], points[i+2], points[i+3], Pal8::white);
  
  for (rauto point: points)
    graphic::font->draw(dst, point, sconv<utf32>(to_str(point, 0)));
}


void bgp_fading_grid(Image& dst, const int bg_state) {
  constx uint GRID_SZ = 32;
  cauto SPEED = bg_state * 4;
  cauto GRID_X = dst.X / GRID_SZ;
  cauto GRID_Y = dst.Y / GRID_SZ;
  xorshift128_state seed {
    .a = 0x2451,
    .b = 0xBEAF,
    .c = 0xDADA,
    .d = 0x2AAF,
  };

  struct Cell {
    bool is_red {};
    real gradient {};
  };
  Vector<Cell> cells(GRID_X * GRID_Y);
  for (rauto cell: cells) {
    cell.is_red = xorshift128(seed) % 2u;
    cell.gradient = std::fmod(xorshift128(seed) + SPEED, 1'000) / 1'000.f;
    // подъём и спад яркости
    if (cell.gradient > 0.5f)
      cell.gradient = 0.5f - (cell.gradient - 0.5f);
    cell.gradient *= 2.f;
  }

  #pragma omp parallel for simd collapse(2)
  cfor (y, GRID_Y)
  cfor (x, GRID_X) {
    cauto cell = cells[y * GRID_X + x];
    cauto is_red = cell.is_red;
    cauto gradient = cell.gradient;
    cfor (gy, GRID_SZ)
    cfor (gx, GRID_SZ)
      dst.set(x * GRID_SZ + gx, y * GRID_SZ + gy, Pal8::from_real(gradient, is_red));
  }
}

void bgp_fading_grid_black(Image& dst, const int bg_state) {
  constx uint GRID_SZ = 16;
  cauto SPEED = bg_state * 3;
  cauto GRID_X = dst.X / GRID_SZ;
  cauto GRID_Y = dst.Y / GRID_SZ;
  xorshift128_state seed {
    .a = 0x2451,
    .b = 0xBEAF,
    .c = 0xDADA,
    .d = 0x2AAF,
  };

  Vector<real> cells(GRID_X * GRID_Y);
  for (rauto cell: cells) {
    cell = ((xorshift128(seed) + SPEED) % 1'000) / 1'000.f;
    // подъём и спад яркости
    if (cell > 0.5f)
      cell = 0.5f - (cell - 0.5f);
    cell *= 0.25f;
  }

  #pragma omp parallel for simd collapse(2)
  cfor (y, GRID_Y)
  cfor (x, GRID_X) {
    cfor (gy, GRID_SZ)
    cfor (gx, GRID_SZ)
      dst.set(x * GRID_SZ + gx, y * GRID_SZ + gy, Pal8::from_real(cells[y * GRID_X + x]));
  }
}

void bgp_fading_grid_red(Image& dst, const int bg_state) {
  constx uint GRID_SZ = 16;
  cauto SPEED = bg_state * 2;
  cauto GRID_X = dst.X / GRID_SZ;
  cauto GRID_Y = dst.Y / GRID_SZ;
  xorshift128_state seed {
    .a = 0x2451,
    .b = 0xBEAF,
    .c = 0xDADA,
    .d = 0x2AAF,
  };

  Vector<real> cells(GRID_X * GRID_Y);
  for (rauto cell: cells) {
    cell = ((xorshift128(seed) + SPEED) % 1'000) / 1'000.f;
    // подъём и спад яркости
    if (cell > 0.5f)
      cell = 0.5f - (cell - 0.5f);
    cell *= 2.f;
  }

  #pragma omp parallel for simd collapse(2)
  cfor (y, GRID_Y)
  cfor (x, GRID_X) {
    cfor (gy, GRID_SZ)
    cfor (gx, GRID_SZ)
      dst.set(x * GRID_SZ + gx, y * GRID_SZ + gy, Pal8::from_real(cells[y * GRID_X + x], true));
  }
}
