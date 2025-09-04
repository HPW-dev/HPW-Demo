#include <cassert>
#include "vrtjoy-visual.hpp"
#include "vrtjoy.hpp"
#include "graphic/image/image.hpp"
#include "game/core/fonts.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"

void draw_vrtjoy(Image& dst, cr<Virtual_joystick> vrtjoy, const Veci pos) {
  // рамка окна
  const Recti wnd {pos, Veci{62, 40}};
  draw_rect_filled<&blend_158>(dst, wnd, Pal8::black);
  draw_rect<&blend_158>(dst, wnd, Pal8::gray);

  // стик
  cauto circle_pos = pos + Vec{wnd.size.y/2, wnd.size.y/2};
  cauto circle_r = wnd.size.y/2 - 4;
  draw_circle<&blend_158>(dst, circle_pos, circle_r, Pal8::white);
  cauto dir = vrtjoy.direction();
  cauto stick_point_r = 3.5;
  draw_circle_filled(dst, circle_pos + dir * (circle_r - stick_point_r), stick_point_r, Pal8::white);

  // зажатые кнопки
  utf32 keys;
  keys += (vrtjoy.pressed(Virtual_joystick::Key::A)      ? U"A" : U" ");
  keys += (vrtjoy.pressed(Virtual_joystick::Key::B)      ? U"B" : U" ");
  keys += U"\n";
  keys += (vrtjoy.pressed(Virtual_joystick::Key::ENABLE) ? U"E" : U" ");
  keys += (vrtjoy.pressed(Virtual_joystick::Key::MENU)   ? U"M" : U" ");
  const Veci keys_pos(circle_pos.x + circle_r + 4, wnd.pos.y + 6);
  assert(graphic::system_mono);
  graphic::system_mono->draw(dst, keys_pos, keys, &blend_diff);
}
