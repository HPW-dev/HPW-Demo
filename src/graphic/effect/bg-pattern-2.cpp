#include <omp.h>
#include <cassert>
#include "bg-pattern.hpp"
#include "graphic/image/image.hpp"
#include "graphic/effect/dither.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/resize.hpp"
#include "game/core/fonts.hpp"
#include "game/core/sprites.hpp"
#include "util/math/random.hpp"

void bgp_warabimochi(Image& dst, const int bg_state) {
  // TODO
}

void bgp_spline(Image& dst, const int bg_state) {
  // TODO
}

void bgp_circle_with_text(Image& dst, const int bg_state) {
  // TODO
}

inline real sigmoid(const real x)
  { return x / (real(1) + std::abs(x)); }

void bgp_dither_wave(Image& dst, const int bg_state) {
  const real state = bg_state * 3.2f;

  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    const real zoom = 0.07f - std::cos(state * 0.0005f) * 0.07f;
    real color = std::cos((x + state) * zoom + y * zoom);
    color *= std::tan((-x + state) * zoom + y * zoom);
    color *= 0.2f;
    dst(x, y) = std::fmod(color * 255.f, 255.f);
  }

  dither_bayer16x16_1bit(dst);
}

void bgp_dither_wave_2(Image& dst, const int bg_state) {
  const real state = bg_state * 3.2f;

  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    const real zoom = 0.07f - std::cos(state * 0.0005f) * 0.07f;
    real color = std::cos((x + state) * zoom + y * zoom);
    color *= std::tan((-x + state) * zoom + y * zoom);
    color *= 0.2f;
    dst(x, y) = std::fmod(color * 255.f, 255.f);
  }

  dither_bayer16x16_1bit(dst);
}

void bgp_tiles_1(Image& dst, const int bg_state) {
  // TODO
}

void bgp_skyline(Image& dst, const int bg_state) {
  cauto skyline = hpw::store_sprite->find("resource/image/loading logo/skyline.png");
  assert(skyline);
  insert_fast(dst, skyline->image());
}

void bgp_tiles_2(Image& dst, const int bg_state) {
  // TODO
}
