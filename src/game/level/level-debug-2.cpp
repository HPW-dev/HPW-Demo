#include <cmath>
#include <omp.h>
#include "level-debug-2.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/game-graphic.hpp"
#include "game/game-core.hpp"
#include "game/game-font.hpp"
#include "game/game-canvas.hpp"
#include "game/util/game-util.hpp"

Level_debug_2::Level_debug_2() {}

void Level_debug_2::update(const Vec vel, double dt) {
  pos_old = pos;
  pos_future_old = pos_future;
  constexpr real speed = 60;
  pos.x += dt * speed;
  pos_future.x = pos.x + dt * speed;

  if (pos.x >= graphic::canvas->X-150)
    pos.x = 0;
}

void Level_debug_2::draw(Image& dst) const {
  constexpr real font_offset = 80;
  Vec pos_default(20, 30 + font_offset * 0);
  Vec pos_defaut_xor(20, 30 + font_offset * 1);
  Vec pos_interpolated(20, 30 + font_offset * 2);
  Vec pos_interpolated_xor(20, 30 + font_offset * 3);
  Vec pos_ups_diff(20, 30 + font_offset * 4);
  dst.fill(Pal8::black);
  graphic::font->draw(dst, pos_default, U"default");
  graphic::font->draw(dst, pos_defaut_xor, U"default xor");
  graphic::font->draw(dst, pos_interpolated, U"interpolated");
  graphic::font->draw(dst, pos_interpolated_xor, U"interpolated xor");
  graphic::font->draw(dst, pos_ups_diff, U"UPF: " + n2s<utf32>(hpw::cur_upf));

  constexpr real r = 10;
  constexpr real offset = 35;
  // default
  draw_circle_filled(dst, ceil(pos_default + pos + offset), r, Pal8::white);
  // default xor
  draw_circle_filled<&blend_xor_safe>(dst, ceil(pos_defaut_xor + pos_old + offset), r, Pal8::white);
  draw_circle_filled<&blend_xor_safe>(dst, ceil(pos_defaut_xor + pos + offset), r, Pal8::white);
  // interpolated
  auto interpolated = pos_old;
  interpolated.x = std::lerp(pos_old.x, pos.x, graphic::lerp_alpha);
  interpolated_old = interpolated;
  draw_circle_filled(dst, ceil(pos_interpolated + interpolated + offset), r, Pal8::white);
  // interpolated xor
  draw_circle_filled<&blend_xor_safe>(dst, ceil(pos_interpolated_xor + interpolated_old + offset), r, Pal8::white);
  draw_circle_filled<&blend_xor_safe>(dst, ceil(pos_interpolated_xor + interpolated + offset), r, Pal8::white);
} // draw
