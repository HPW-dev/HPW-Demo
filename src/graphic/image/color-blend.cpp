#include <cassert>
#include <cmath>
#include "color-blend.hpp"
#include "game/game-graphic.hpp"

inline double get_state(const Pal8 src, int optional, double speed) {
  double state = src.to_real() + graphic::effect_state * speed;
  return state + optional * 0.137;
}

[[nodiscard, gnu::const]] Pal8 blend_rotate(const Pal8 in, const Pal8 bg, int optional)
  { return Pal8(in.val + get_state(in, optional, 1) * 255.0); }

[[nodiscard, gnu::const]] Pal8 blend_rotate_x4(const Pal8 in, const Pal8 bg, int optional)
  { return Pal8(in.val + get_state(in, optional, 4) * 255.0); }

[[nodiscard, gnu::const]] Pal8 blend_rotate_x16(const Pal8 in, const Pal8 bg, int optional)
  { return Pal8(in.val + get_state(in, optional, 16) * 255.0); }

[[nodiscard, gnu::const]] Pal8 blend_rotate_safe(const Pal8 in, const Pal8 bg, int optional)
  { return Pal8::from_real(std::fmod(get_state(in, optional, 1), 1.0), in.is_red()); }

[[nodiscard, gnu::const]] Pal8 blend_rotate_x4_safe(const Pal8 in, const Pal8 bg, int optional)
  { return Pal8::from_real(std::fmod(get_state(in, optional, 4), 1.0), in.is_red()); }

[[nodiscard, gnu::const]] Pal8 blend_rotate_x16_safe(const Pal8 in, const Pal8 bg, int optional)
  { return Pal8::from_real(std::fmod(get_state(in, optional, 16), 1.0), in.is_red()); }
