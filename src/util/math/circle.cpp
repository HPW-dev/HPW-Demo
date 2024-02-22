#include <utility>
#include <cassert>
#include "circle.hpp"

#include "util/math/mat.hpp"

#ifdef CLD_DEBUG
#include "game/core/debug.hpp"
#endif

inline double fast_distance(const Vec pos_a, CN<Circle> a,
const Vec pos_b, CN<Circle> b) {
  return
    pow2((pos_a.x + a.offset.x) - (pos_b.x + b.offset.x)) +
    pow2((pos_a.y + a.offset.y) - (pos_b.y + b.offset.y));
}

bool Circle::operator ==(CN<Circle> other) const {
  return this->offset == other.offset && this->r == other.r;
}

bool Circle::operator !=(CN<Circle> other) const {
  return this->offset != other.offset || this->r != other.r;
}

Circle::operator bool() const {
  return r != 0 || offset;
}

bool Circle::is_collided(const Vec this_pos, const Vec other_pos, CN<Circle> other) const {
  // себя не проверять
  if (*this == other)
    return false;
    
  #ifdef CLD_DEBUG
  ++hpw::circle_checks;
  #endif
  
  return fast_distance(this_pos, *this, other_pos, other) < pow2(this->r + other.r);
}
