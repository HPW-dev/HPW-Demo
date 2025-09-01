#include <cassert>
#include "display-3d.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/resize.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"

namespace epge {

struct Display_3d::Impl final {
  enum class Blend_mode {
    _xor = 0,
    _or,
    _plus,
    _max,
    _avr,
    MAX,
  };
  enum class Resize_mode {
    neighbor = 0,
    bilinear,
    MAX,
  };
  int _resize_mode {scast<int>(Resize_mode::neighbor)};
  int _blend_mode {scast<int>(Blend_mode::_plus)};
  double _depth {0.988};
  double _bg_shading {0.74};
  mutable Image _resized {};
  mutable Image _dst_copy {};

  inline Str name() const noexcept { return "3D display"; }
  inline Str desc() const noexcept { return "simulates the depth of mobile phone LCD display"; }

  inline void draw(Image& dst) const noexcept {
    assert(dst);

    // заресайзить
    cauto new_x = dst.X*_depth;
    cauto new_y = dst.Y*_depth;
    switch (scast<Resize_mode>(_resize_mode)) {
      default:
      case Resize_mode::neighbor: _resized = resize_neighbor(dst, new_x, new_y); break;
      case Resize_mode::bilinear: _resized = resize_bilinear(dst, new_x, new_y); break;
    }

    _dst_copy = dst;
    dst.fill(Pal8::black);
    insert(dst, _resized, center_point(dst, _resized));
    sub_brightness(dst, Pal8::from_real(_bg_shading));
    insert(dst, _dst_copy, Vec{}, get_blend_mode(scast<Blend_mode>(_blend_mode)));
  }

  inline Params params() noexcept {
    return Params {
      new_shared<Param_double>("depth", "distance of pixels from the screen", _depth, 0.9, 0.999, 0.001, 0.01),
      new_shared<Param_double>("Bg shading", "screen shading intensity", _bg_shading, 0.001, 0.999, 0.001, 0.01),
      new_shared<Param_int>("mode", "resize algorithm:\n"
        "  0 - neighbor;\n  1 - bilinear.\n", _resize_mode, 0, scast<int>(Resize_mode::MAX)-1, 1, 1),
      new_shared<Param_int>("blend mode", "pixel blending modes:\n"
        "  0 - XOR, 1 - |, 2 - +,\n"
        "  3 - max, 5 - average", _blend_mode, 0, scast<int>(Blend_mode::MAX)-1, 1, 1),
    };
  }

  inline static blend_pf get_blend_mode(const Blend_mode blend_mode) {
    switch (blend_mode) {
      default:
      case Blend_mode::_xor: return &blend_xor_safe;
      case Blend_mode::_or: return &blend_or_safe;
      case Blend_mode::_plus: return &blend_add_safe;
      case Blend_mode::_max: return &blend_max;
      case Blend_mode::_avr: return &blend_avr;
    }
    return &blend_past;
  }
}; // Impl

EPGE_IMPL_MAKER(Display_3d)

} // epge ns
