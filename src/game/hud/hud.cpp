#include "hud.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "util/math/vec.hpp"
#include "game/util/sync.hpp"
#include "game/core/fonts.hpp"
#include "game/core/difficulty.hpp"

void Hud::draw_expanded_text(Image& dst, cr<utf32> txt, const Vec pos) {
  // рендер в буфферы под текст
  Image hp_overlay(graphic::font->text_width(txt) + 2,
    graphic::font->text_height(txt) + 2, Pal8::black);
  graphic::font->draw(hp_overlay, {1, 1}, txt);

  // расширение контуров текста
  Image hp_overlay_black(hp_overlay);
  apply_invert(hp_overlay_black);
  expand_color_8(hp_overlay_black, Pal8::black);

  // вставка тёмного контура текста
  insert_blink<&blend_min>(dst, hp_overlay_black, pos, graphic::frame_count);
  // вставка текста
  if (hpw::difficulty == Difficulty::easy)
    insert_blink<&blend_max>(dst, hp_overlay, pos, graphic::frame_count);
  else
    insert<&blend_max>(dst, hp_overlay, pos, graphic::frame_count);
} // draw_expanded_text
