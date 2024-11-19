#include <omp.h>
#include "scanline.hpp"
#include "graphic/image/color-blend.hpp"
#include "graphic/image/image.hpp"

namespace epge {

struct Scanline::Impl final {
  double _shadow {0.25}; // степень затенения полосками

  inline Str name() const noexcept { return "scanline"; }
  inline Str desc() const noexcept { return "CRT display scanline simulation"; }

  inline void draw(Image& dst) const noexcept {
    cauto SCANLINE_SHADOW = Pal8::from_real(_shadow);

    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (y, dst.Y)
      if (y % 2)
        cfor (x, dst.X)
          dst(x, y) = blend_sub_safe(SCANLINE_SHADOW, dst(x, y));
  }

  inline epge::Params params() noexcept {
    return epge::Params {
      new_shared<epge::Param_double>("shadow", "degree of shading by lines", _shadow, 0, 1, 0.01, 0.1),
    };
  }
}; // Impl

Scanline::Scanline(): impl{new_unique<Impl>()} {}
Scanline::~Scanline() {}
Str Scanline::name() const noexcept { return impl->name(); }
Str Scanline::desc() const noexcept { return impl->desc(); }
void Scanline::draw(Image& dst) const noexcept { impl->draw(dst); }
Params Scanline::params() noexcept { return impl->params(); }

} // epge ns
