#include "vrtjoy-visual.hpp"
#include "vrtjoy.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/math/random.hpp" // TODO DEL

void draw_vrtjoy(Image& dst, cr<Virtual_joystick> vrtjoy, const Veci pos) {
  // рамка окна
  const Recti wnd {pos, Veci{90, 40}};
  draw_rect_filled<&blend_158>(dst, wnd, Pal8::black);
  draw_rect<&blend_158>(dst, wnd, Pal8::gray);

  // стик
  cauto circle_pos = pos + Vec{wnd.size.y/2, wnd.size.y/2};
  cauto circle_r = wnd.size.y/2 - 4;
  draw_circle<&blend_158>(dst, circle_pos, circle_r, Pal8::white);
  //cauto dir = vrtjoy.direction(); // TODO resurect
  cauto dir = rand_normalized_graphic(); // TODO DEL
  cauto stick_point_r = 3.5;
  // TODO рандом радиус убрать
  draw_circle_filled(dst, circle_pos + dir * rndr(0, circle_r - stick_point_r), stick_point_r, Pal8::white);
}
