#include <cassert>
#include "crazy-blur.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/resize.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "util/math/random.hpp"

namespace epge {

struct Crazy_blur::Impl final {
  double _power {0.021};
  int _framents {3};
  int _side {2};
  int _interp_mode {0};
  mutable Image fragment_a {};
  mutable Image side_a {};
  mutable Image fragment_b {};
  mutable Image side_b {};

  inline Str name() const noexcept { return "crazy blur"; }
  inline Str desc() const noexcept { return "randomly resize fragments of image"; }
  
  inline void draw(Image& dst) const noexcept {
    assert(dst);
    auto resize_f = _interp_mode == 0 ? &resize_neighbor : &resize_bilinear;
    
    // hor
    if (_side == 0 || _side == 2) {
      for (int x = 0; x < _framents-1; ++x) {
        cauto divider = rnd_fast(3, dst.X-3);
        cauto shifted = std::clamp<int>(divider + dst.X * rndr_fast(-_power, _power), 3, dst.X-3);
        cont_if (divider == shifted);
        fragment_a = cut(dst, {0, 0, divider, dst.Y});
        side_a = resize_f(fragment_a, shifted, dst.Y);
        fragment_b = cut(dst, {divider, 0, dst.X-divider-1, dst.Y});
        side_b = resize_f(fragment_b, dst.X-shifted-1, dst.Y);
        insert(dst, side_a, {0,0});
        insert(dst, side_b, {shifted,0});
      }
    }

    // vert
    if (_side == 1 || _side == 2) {
      for (int y = 0; y < _framents-1; ++y) {
        cauto divider = rnd_fast(3, dst.Y-3);
        cauto shifted = std::clamp<int>(divider + dst.Y * rndr_fast(-_power, _power), 3, dst.Y-3);
        cont_if (divider == shifted);
        fragment_a = cut(dst, {0, 0, dst.X, divider});
        side_a = resize_f(fragment_a, dst.X, shifted);
        fragment_b = cut(dst, {0, divider, dst.X, dst.Y-divider-1});
        side_b = resize_f(fragment_b, dst.X, dst.Y-shifted-1);
        insert(dst, side_a, {0,0});
        insert(dst, side_b, {0,shifted});
      }
    }
  }

  inline Params params() noexcept {
    return Params {
      new_shared<Param_int>("fragments", "count of image fragments", _framents, 2, 16, 1, 2),
      new_shared<Param_double>("power", "power of blur", _power, 0.001, 0.999, 0.001, 0.02),
      new_shared<Param_int>("side", "side mode: 0-horizontal, 1-vertical, 2-all", _side, 0, 2, 1, 2),
      new_shared<Param_int>("mode", "interpolation mode: 0-neighbor, 1-bilinear", _interp_mode, 0, 1, 1, 2),
    };
  }
}; // Impl

EPGE_IMPL_MAKER(Crazy_blur)

} // epge ns
