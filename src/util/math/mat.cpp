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

int32_t digits_number_i32(int32_t x) {  
  x = std::abs(x);  
  return
    (x <            10 ? 1 :   
    (x <           100 ? 2 :   
    (x <         1'000 ? 3 :   
    (x <        10'000 ? 4 :   
    (x <       100'000 ? 5 :   
    (x <     1'000'000 ? 6 :   
    (x <    10'000'000 ? 7 :  
    (x <   100'000'000 ? 8 :  
    (x < 1'000'000'000 ? 9 : 10
    )))))))));  
} 
