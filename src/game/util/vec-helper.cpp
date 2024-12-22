#include <cassert>
#include "vec-helper.hpp"
#include "game/core/canvas.hpp"
#include "util/math/vec.hpp"
#include "util/math/rect.hpp"
#include "util/math/random.hpp"

Vec get_screen_center() { return Vec(graphic::width / 2.0, graphic::height / 2.0); }

Vec get_rand_pos_safe(const real sx, const real sy, const real ex, const real ey) {
  return Vec(
    rndr(sx, ex),
    rndr(sy, ey)
  );
}

Vec get_rand_pos_graphic(const real sx, const real sy, const real ex, const real ey) {
  return Vec(
    rndr_fast(sx, ex),
    rndr_fast(sy, ey)
  );
}

Vec rnd_screen_pos_safe() {
  assert(graphic::canvas);
  return Vec {
    rndr(0, graphic::canvas->X),
    rndr(0, graphic::canvas->Y)
  };
}

Vec rnd_screen_pos_fast() {
  assert(graphic::canvas);
  return Vec {
    rndr_fast(0, graphic::canvas->X),
    rndr_fast(0, graphic::canvas->Y)
  };
}

[[nodiscard]] Rect get_screen_rect() { return Rect(0,0, graphic::canvas->X,graphic::canvas->Y); }
