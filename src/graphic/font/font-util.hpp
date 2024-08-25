#pragma once
#include "graphic/image/color-blend.hpp"
#include "util/macro.hpp"
#include "util/unicode.hpp"
#include "util/math/rect.hpp"
#include "util/math/vec.hpp"

namespace hpw { class Font; }
class Image;

// нарисовать текст по центру
void draw_center(cp<hpw::Font> font, Image& dst,
  cr<utf32> msg, blend_pf bf=&blend_max);

// рисует текст в пределах прямоугольной области
void text_bordered(Image& dst, cr<utf32> txt, cp<hpw::Font> font,
  const Rect border, const Vec offset, blend_pf bf=&blend_past, int opt=0);
