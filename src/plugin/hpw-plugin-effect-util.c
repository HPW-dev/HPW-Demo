#include <math.h>
#include "hpw-plugin-effect-util.h"

struct Rgb24 pal8_to_rgb24(const Pal8 src) {
  struct Rgb24 ret;
  if (src == 255u) {
    ret.r = ret.g = ret.b = 0xFFu;
    return ret;
  }
  if ( !(src > 222 && src <= 254) ) {
    const double GRAY_MUL = 255.0 / 222.0;
    ret.r = ret.g = ret.b = round(src * GRAY_MUL);
    return ret;
  }

  ret.g = ret.b = 0;
  const double RED_MUL = 255.0 / 32.0;
  ret.r = round((src - 222) * RED_MUL);
  return ret;
}

Pal8 rgb24_to_pal8(const struct Rgb24 src) {
  // если R оттенок сильнее G и B, то это красный цвет с палитры
  if (src.r > 10 + ((src.g + src.b) >> 1)) {
    const double RED_MUL = 32.0 / 255.0;
    return 222 + round(src.r * RED_MUL);
  } else if ((src.g & src.b & src.r & 0xFFu) == 0xFFu) { // полностью белый
    return 255u; // white
  }

  // всё остальное, это серый
  const double GRAY_MUL = 222.0 / 255.0;
  const double gray = src.r * 0.299 + src.g * 0.587 + src.b * 0.114;
  return round(gray * GRAY_MUL);
}
