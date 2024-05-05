#pragma once
#include "util/macro.hpp"
#include "graphic/image/color-blend.hpp"
#include "util/unicode.hpp"

namespace hpw { class Font; }
class Image;

// нарисовать текст по центру
void draw_center(CP<hpw::Font> font, Image& dst,
  CN<utf32> msg, blend_pf bf=&blend_max);
