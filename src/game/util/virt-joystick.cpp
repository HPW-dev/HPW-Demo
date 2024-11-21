#include "virt-joystick.hpp"
#include "game/core/fonts.hpp"
#include "game/util/keybits.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "util/math/vec.hpp"
#include "util/math/rect.hpp"
#include "util/unicode.hpp"

void draw_virtual_joystick(Image& dst) {
  const Vec pos (5, 300);
  const Vec text_offset (5, 5);
  draw_rect_filled<&blend_158>(dst, Rect(pos.x, pos.y, 65, 55), Pal8::black);

  utf32 inputs;
  inputs += is_pressed(hpw::keycode::mode)  ? U"M" : U"_";
  inputs += is_pressed(hpw::keycode::shoot) ? U"S" : U"_";
  inputs += U"  ";
  inputs += is_pressed(hpw::keycode::up)    ? U"#" : U"_";
  inputs += U" \n";

  inputs += U" ";
  inputs += is_pressed(hpw::keycode::bomb)  ? U"B" : U"_";
  inputs += U" ";
  inputs += is_pressed(hpw::keycode::left)  ? U"#" : U"_";
  inputs += is_pressed(hpw::keycode::down)  ? U"#" : U"_";
  inputs += is_pressed(hpw::keycode::right) ? U"#" : U"_";
  inputs += U" \n";

  inputs += U" ";
  inputs += is_pressed(hpw::keycode::focus) ? U"F" : U"_";
  graphic::font->draw(dst, pos + text_offset, inputs);
}
