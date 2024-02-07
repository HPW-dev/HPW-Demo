#include <cmath>
#include "mat.hpp"

real rad_to_deg(real rad) {
  constexpr auto CONST = 180 / PI;
  return rad * CONST;
}

real deg_to_rad(real deg) {
  constexpr auto CONST = PI / 180;
  return deg * CONST;
}

real ring_rad(real rad) {
  constexpr auto maxRad = PI * 2;
  if (rad < 0)
    while (rad < 0)
      rad += maxRad;
  else
    while (rad >= maxRad)
      rad -= maxRad;
  return rad;
}

real ring_deg(real deg) {
  if (deg < 0)
    return 360.0 - std::fmod(std::abs(deg), real(360));
  return std::fmod(deg, real(360));
}

real get_sign(real val) { return std::copysign(real(1), val); }
