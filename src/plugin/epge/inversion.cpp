#include <omp.h>
#include <cassert>
#include "inversion.hpp"
#include "graphic/image/color-blend.hpp"
#include "graphic/image/image.hpp"

namespace epge {

struct Inversion::Impl final {
  bool _with_glitchez {};

  inline Str name() const noexcept { return "inversion"; }
  inline Str desc() const noexcept { return "color inversion"; }

  inline void draw(Image& dst) const noexcept {
    assert(dst);
    
    if (_with_glitchez) {    
      #pragma omp parallel for simd if (dst.size > 64 * 64)
      cfor (i, dst.size)
        dst[i].val = ~dst[i].val;
    } else {
      #pragma omp parallel for simd if (dst.size > 64 * 64)
      cfor (i, dst.size)
        dst[i] = blend_inv_safe(dst[i]);
    }
  }

  inline epge::Params params() noexcept {
    return epge::Params {
      new_shared<epge::Param_bool>("with glitchez", "less accurate", _with_glitchez),
    };
  }
}; // Impl

Inversion::Inversion(): impl{new_unique<Impl>()} {}
Inversion::~Inversion() {}
Str Inversion::name() const noexcept { return impl->name(); }
Str Inversion::desc() const noexcept { return impl->desc(); }
void Inversion::draw(Image& dst) const noexcept { impl->draw(dst); }
Params Inversion::params() noexcept { return impl->params(); }

} // epge ns
