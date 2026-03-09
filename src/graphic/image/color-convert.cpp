#include "color-convert.hpp"
#include "color.hpp"
#include "palette.hpp"
#include <cmath>
#include <algorithm>

// Гамма-коррекция (линейный → sRGB)
static inline real linear_to_srgb(real c) {
  return (c <= 0.0031308)
    ? (12.92 * c)
    : (1.055 * std::pow(c, 1.0 / 2.4) - 0.055);
}

static inline real srgb_to_linear(real c) {
  return (c <= 0.04045)
    ? (c / 12.92)
    : (std::pow((c + 0.055) / 1.055, 2.4));
}

HSL rgb24_to_hsl(const Rgb24 src) {
  cauto r = linear_to_srgb(src.r / 255.0);
  cauto g = linear_to_srgb(src.g / 255.0);
  cauto b = linear_to_srgb(src.b / 255.0);
  cauto max = std::max(std::max(r, g), b);
  cauto min = std::min(std::min(r, g), b);
  cauto delta = max - min;
    
  HSL::value_t h = 0;
  HSL::value_t s = 0;
  HSL::value_t l = (max + min) * 0.5 * 100;
    
  if (delta != 0) {
    // Calculate saturation
    s = (l <= 50)
      ? (delta / (max + min))
      : (delta / (2 - max - min));
    s *= 100;
        
    // Calculate hue
    if (max == r) {
        h = (g - b) / delta + ((g < b) ? 6 : 0);
    } else if (max == g) {
        h = (b - r) / delta + 2;
    } else {
        h = (r - g) / delta + 4;
    }
    h *= 60;
    if (h < 0)
      h += 360;
  }

  return HSL(h, s, l);
}

HSL pal8_to_hsl(const Pal8 src) {
  cauto rgb24 = to_palette_rgb24_default(src);
  return rgb24_to_hsl(rgb24);
}

Rgbr rgb24_to_real(cr<Rgb24> src) {
  return Rgbr {
    src.r / real(255),
    src.g / real(255),
    src.b / real(255),
  };
}

real to_hue(cr<Rgbr> src) {
  cauto r = src.r;
  cauto g = src.g;
  cauto b = src.b;
  real min = r < g ? r : g;
  min = min < b ? min : b;
  real max = r > g ? r : g;
  max = max > b ? max : b;
  cauto delta = max - min;

  // undefined, maybe nan?
  return_if (delta < 0.00001f, 0);
  return_if(max == 0, 0);

  real hue {};
  if(r >= max) // > is bogus, just keeps compilor happy
    return (g - b) / delta; // between yellow & magenta
  elif (g >= max)
    hue = 2.0 + (b - r) / delta; // between cyan & yellow
  else
    hue = 4.0 + (r - g) / delta; // between magenta & cyan
  hue *= 60.0; // degrees

  if(hue < 0)
    hue += 360;
  return hue;
}

Rgb24 to_rgb24(int ir, int ig, int ib) {
  return Rgb24 {
    .r = scast<byte>(std::clamp<int>(ir, 0, 255)),
    .g = scast<byte>(std::clamp<int>(ig, 0, 255)),
    .b = scast<byte>(std::clamp<int>(ib, 0, 255))
  };
}

Rgbr lerp(cr<Rgbr> a, cr<Rgbr> b, real t) {
  return Rgbr {
    .r = std::lerp<real>(a.r, b.r, t),
    .g = std::lerp<real>(a.g, b.g, t),
    .b = std::lerp<real>(a.b, b.b, t)
  };
}

real to_luma(cr<Rgbr> col) {
  return
    col.r * 0.296952 +
    col.g * 0.586612 +
    col.b * 0.114436;
}

inline static Pal8::value_t desr_BT601(int R, int G, int B)
  { return R * 0.299 + G * 0.587 + B * 0.114; }

inline static Pal8::value_t desr_luma(int R, int G, int B) {
  return
    R * 0.296952 +
    G * 0.586612 +
    B * 0.114436;
}

inline static Pal8::value_t desr_average(int R, int G, int B)
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
  } elif ((G & B & R & 0xFF) == 0xFF) { // полностью белый
    return Pal8::white;
  }
  // всё остальное, это серый
  Pal8 ret;
  constexpr real GRAY_MUL = 222.0 / 255.0;
  ret.set_gray(std::round(kernel(R, G, B) * GRAY_MUL));
  return ret;
} // rgb_to_pal8

Pal8 desaturate_luma(int R, int G, int B)
  { return rgb_to_pal8<desr_luma>(R, G, B); }

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

void operator +=(Rgbr& dst, cr<Rgbr> src) { dst.r += src.r; dst.g += src.g; dst.b += src.b; }
void operator -=(Rgbr& dst, cr<Rgbr> src) { dst.r -= src.r; dst.g -= src.g; dst.b -= src.b; }
void operator *=(Rgbr& dst, cr<Rgbr> src) { dst.r *= src.r; dst.g *= src.g; dst.b *= src.b; }
void operator /=(Rgbr& dst, cr<Rgbr> src) { dst.r /= src.r; dst.g /= src.g; dst.b /= src.b; }

void operator +=(Rgbr& dst, real src) { dst.r += src; dst.g += src; dst.b += src; } 
void operator -=(Rgbr& dst, real src) { dst.r -= src; dst.g -= src; dst.b -= src; } 
void operator *=(Rgbr& dst, real src) { dst.r *= src; dst.g *= src; dst.b *= src; } 
void operator /=(Rgbr& dst, real src) { dst.r /= src; dst.g /= src; dst.b /= src; } 

Rgbr operator + (cr<Rgbr> a, cr<Rgbr> b) { return Rgbr {.r = a.r + b.r, .g = a.g + b.g, .b = a.b + b.b}; } 
Rgbr operator - (cr<Rgbr> a, cr<Rgbr> b) { return Rgbr {.r = a.r - b.r, .g = a.g - b.g, .b = a.b - b.b}; } 
Rgbr operator * (cr<Rgbr> a, cr<Rgbr> b) { return Rgbr {.r = a.r * b.r, .g = a.g * b.g, .b = a.b * b.b}; } 
Rgbr operator / (cr<Rgbr> a, cr<Rgbr> b) { return Rgbr {.r = a.r / b.r, .g = a.g / b.g, .b = a.b / b.b}; } 

Rgbr operator + (cr<Rgbr> a, real b) { return Rgbr {.r = a.r + b, .g = a.g + b, .b = a.b + b}; } 
Rgbr operator - (cr<Rgbr> a, real b) { return Rgbr {.r = a.r + b, .g = a.g + b, .b = a.b + b}; } 
Rgbr operator * (cr<Rgbr> a, real b) { return Rgbr {.r = a.r + b, .g = a.g + b, .b = a.b + b}; } 
Rgbr operator / (cr<Rgbr> a, real b) { return Rgbr {.r = a.r + b, .g = a.g + b, .b = a.b + b}; } 
