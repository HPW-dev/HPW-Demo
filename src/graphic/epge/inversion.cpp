#include <omp.h>
#include <cassert>
#include "epge.hpp"
#include "graphic/image/color-blend.hpp"
#include "graphic/image/image.hpp"
#include "game/util/locale.hpp"

EPGE_CLASS_BEGIN(inversion)
  bool _with_glitchez {};

public:
  inline void draw(Image& dst) const {
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

  inline epge::Params params() {
    return epge::Params {
      new_shared<epge::Param_bool>("with_glitchez", U"with glitchez", U"less accurate", _with_glitchez),
    };
  }
EPGE_CLASS_END(inversion)
