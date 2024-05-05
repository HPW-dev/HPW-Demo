#include <cmath>
#include "convert.hpp"
#include "graphic/image/color.hpp"

static Pal8::value_t desr_BT601(int R, int G, int B)
  { return R * 0.299 + G * 0.587 + B * 0.114; }

static Pal8::value_t desr_average(int R, int G, int B)
  { return (R + G + B) / 3; }

// преобразование в палитру hpw
template <Pal8::value_t (*kernel)(int R, int G, int B)>
Pal8 rgb_to_pal8(int R, int G, int B) {
  // если R оттенок сильнее G и B, то это красный цвет с палитры
  if (R > 10 + ((G + B) >> 1)) {
    Pal8 ret;
    constexpr real RED_MUL = 32.0 / 255.0;
    ret.set_red(std::round(R * RED_MUL));
    return ret;
  } else if ((G & B & R & 0xFF) == 0xFF) { // полностью белый
    return Pal8::white;
  }
  // всё остальное, это серый
  Pal8 ret;
  constexpr real GRAY_MUL = 222.0 / 255.0;
  ret.set_gray(std::round(kernel(R, G, B) * GRAY_MUL));
  return ret;
} // rgb_to_pal8

Pal8 desaturate_bt601(int R, int G, int B)
  { return rgb_to_pal8<desr_BT601>(R, G, B); }

Pal8 desaturate_average(int R, int G, int B)
  { return rgb_to_pal8<desr_average>(R, G, B); }

Rgb24 to_rgb24(const Pal8 x) {
  // TODO можно сделать быструю версию с таблицей перекодировки
  Rgb24 ret;
  if (x.is_white()) {
    ret.r = ret.g = ret.b = 0xFF;
    return ret;
  }
  if ( !x.is_red()) {
    constexpr real GRAY_MUL = 255.0 / Pal8::gray_size;
    ret.r = ret.g = ret.b = std::round(x.val * GRAY_MUL);
    return ret;
  }
  ret.g = ret.b = 0;
  constexpr real RED_MUL = 255.0 / Pal8::red_size;
  ret.r = std::round((x.val - Pal8::red_start) * RED_MUL);
  return ret;
} // to_rgb24
