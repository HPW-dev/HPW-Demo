#include <omp.h>
#include <cassert>
#include "inversion.hpp"
#include "graphic/image/color-blend.hpp"
#include "graphic/image/image.hpp"
#include "game/util/locale.hpp"

namespace epge {

struct Inversion::Impl final {
  bool _with_glitchez {};

  inline Str name() const noexcept { return "inversion"; }
  #define LOCSTR(NAME) get_locale_str("epge.effect.inversion." NAME)
  inline utf32 localized_name() const { return LOCSTR("name"); }
  inline utf32 desc() const noexcept { return LOCSTR("desc"); }

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

EPGE_IMPL_MAKER(Inversion)

} // epge ns
