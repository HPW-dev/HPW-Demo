#include "font-util.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "util/math/vec.hpp"

void draw_center(CP<hpw::Font> font, Image& dst,
CN<utf32> msg, blend_pf bf) {
  auto w = font->text_width(msg);
  auto h = font->text_height(msg);
  Vec pos{
    dst.X / 2 - w / 2,
    dst.Y / 2 - h / 2,
  };
  font->draw(dst, pos, msg, bf);
}
