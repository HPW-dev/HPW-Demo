#pragma once
#include "graphic/image/color-blend.hpp"
#include "util/macro.hpp"
#include "util/unicode.hpp"
#include "util/math/rect.hpp"
#include "util/math/vec.hpp"

class Font_base;
class Image;

// нарисовать текст по центру
void draw_center(cp<Font_base> font, Image& dst,
  cr<utf32> msg, blend_pf bf=&blend_max);

// рисует текст в пределах прямоугольной области
void text_bordered(Image& dst, cr<utf32> txt, cp<Font_base> font,
  const Rect border, const Veci offset, blend_pf bf=&blend_past, int opt=0);
