#include "color-convert.hpp"
#include "color.hpp"
#include "palette.hpp"
#include <cmath>

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
