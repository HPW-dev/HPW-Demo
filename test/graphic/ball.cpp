#include "ball.hpp"
#include "game/game-canvas.hpp"
#include "util/math/random.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"

Ball::Ball(real speed_, const Pal8 color_)
: color(color_), speed(speed_) {
  random_init();
}

void Ball::random_init() {
  // шар в случайной позиции
  pos.x = rndr(0, graphic::canvas->X-1);
  pos.y = rndr(0, graphic::canvas->Y-1);
  dir.x = (rndb() & 1) ? -1 : 1;
  dir.y = (rndb() & 1) ? -1 : 1;
}

void Ball::move(double dt) {
  pos.x += dir.x * speed * dt;
  if (pos.x < 0) {
    dir.x *= -1;
    pos.x = 0;
  }
  if (pos.x >= graphic::canvas->X) {
    dir.x *= -1;
    pos.x = graphic::canvas->X - 1;
  }
  pos.y += dir.y * speed * dt;
  if (pos.y < 0) {
    dir.y *= -1;
    pos.y = 0;
  }
  if (pos.y >= graphic::canvas->Y) {
    dir.y *= -1;
    pos.y = graphic::canvas->Y - 1;
  }
} // Ball::move

void Ball::draw(Image& dst) const {
  int radius = 15 + rnd(0, 10);
  draw_circle_filled<blend_diff>(dst, pos,
    radius + rnd(1, 10), Pal8::white);
  draw_circle_filled(dst, pos, radius, color);
}
