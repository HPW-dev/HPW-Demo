#include <cassert>
#include "crazy-blur.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/resize.hpp"
#include "util/math/random.hpp"

namespace epge {

struct Crazy_blur::Impl final {
  double _power {0.1};
  int _framents {2};
  int _side {0};
  int _interp_mode {0};

  inline Str name() const noexcept { return "crazy blur"; }
  inline Str desc() const noexcept { return "randomly resize fragments of image"; }
  
  inline void draw(Image& dst) const noexcept {
    assert(dst);
  }

  inline Params params() noexcept {
    return Params {
      new_shared<Param_int>("fragments", "count of image fragments", _framents, 2, 16, 1, 2),
      new_shared<Param_double>("power", "power of blur", _power, 0.01, 0.99, 0.01, 0.1),
      new_shared<Param_int>("side", "side mode: 0-horizontal, 1-vertical, 2-all", _side, 0, 2, 1, 2),
      new_shared<Param_int>("mode", "interpolation mode: 0-neighbor, 1-bilinear, 2-bicubic", _interp_mode, 0, 2, 1, 2),
    };
  }
}; // Impl

Crazy_blur::Crazy_blur(): impl{new_unique<Impl>()} {}
Crazy_blur::~Crazy_blur() {}
Str Crazy_blur::name() const noexcept { return impl->name(); }
Str Crazy_blur::desc() const noexcept { return impl->desc(); }
void Crazy_blur::draw(Image& dst) const noexcept { impl->draw(dst); }
Params Crazy_blur::params() noexcept { return impl->params(); }

} // epge ns
