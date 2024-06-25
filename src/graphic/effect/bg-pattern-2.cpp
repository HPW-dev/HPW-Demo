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

void bgp_dither_wave(Image& dst, const int bg_state) {
  // TODO
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
