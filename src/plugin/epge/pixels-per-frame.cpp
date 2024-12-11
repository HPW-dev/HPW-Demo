#include <omp.h>
#include "pixels-per-frame.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"

namespace epge {

struct Pixels_per_frame::Impl final {
  int _ppf {800};

  inline Str name() const noexcept { return "pixels per frame"; }
  inline Str desc() const noexcept { return "draws a certain number of pixels per frame"; }

  inline void draw(Image& dst) const noexcept {
    assert(dst);
    // TOD
  }

  inline epge::Params params() noexcept {
    return epge::Params {
      new_shared<epge::Param_int>("pixels per frame", "number of pixels per frame", _ppf, 1, 512*384 - 1, 4, 40),
    };
  }
}; // Impl

Pixels_per_frame::Pixels_per_frame(): impl{new_unique<Impl>()} {}
Pixels_per_frame::~Pixels_per_frame() {}
Str Pixels_per_frame::name() const noexcept { return impl->name(); }
Str Pixels_per_frame::desc() const noexcept { return impl->desc(); }
void Pixels_per_frame::draw(Image& dst) const noexcept { impl->draw(dst); }
Params Pixels_per_frame::params() noexcept { return impl->params(); }

} // epge ns
