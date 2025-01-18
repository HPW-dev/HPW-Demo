#include <omp.h>
#include <cassert>
#include <algorithm>
#include "bg-pattern-physics.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/resize.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/math/xorshift.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/mat.hpp"
#include "util/str-util.hpp"

void bgp_physics_1(Image& dst, const int bg_state) {
  assert(dst);
}
