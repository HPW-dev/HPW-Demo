#include <utility>
#include <cassert>
#include "circle.hpp"

#include "util/math/mat.hpp"

#ifdef CLD_DEBUG
#include "game/core/debug.hpp"
#endif

inline real fast_distance(const Vec pos_a, cr<Circle> a,
const Vec pos_b, cr<Circle> b) {
  return
    pow2((pos_a.x + a.offset.x) - (pos_b.x + b.offset.x)) +
    pow2((pos_a.y + a.offset.y) - (pos_b.y + b.offset.y));
}

bool Circle::operator ==(cr<Circle> other) const {
  return this->offset == other.offset && this->r == other.r;
}

bool Circle::operator !=(cr<Circle> other) const {
  return this->offset != other.offset || this->r != other.r;
}

Circle::operator bool() const {
  return r != 0 || offset.not_zero();
}

bool Circle::is_collided(const Vec this_pos, const Vec other_pos, cr<Circle> other) const {    
  #ifdef CLD_DEBUG
  ++hpw::circle_checks;
  #endif
  
  return fast_distance(this_pos, *this, other_pos, other) < pow2(this->r + other.r);
}
