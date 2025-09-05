#include <omp.h>
#include <cassert>
#include "epge.hpp"
#include "graphic/image/color-blend.hpp"
#include "graphic/image/image.hpp"
#include "game/util/locale.hpp"

namespace epge {

#define EFFECT_NAME inversion
#define CLASS_NAME CONCAT(Epge_, EFFECT_NAME)
#define REGISTRATOR_NAME CONCAT(_registrator_for_, EFFECT_NAME)
#define EFFECT_NAME_STR STRINGIFY(EFFECT_NAME)
#define LOCSTR(NAME) get_locale_str("epge.effect." EFFECT_NAME_STR "." NAME)

class CLASS_NAME: public epge::Base {
  bool _with_glitchez {};

public:
  inline Str name() const { return EFFECT_NAME_STR; }
  inline utf32 localized_name() const { return LOCSTR("name"); }
  inline utf32 desc() const { return LOCSTR("desc"); }

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
}; // class

inline Epge_registrator<CLASS_NAME> REGISTRATOR_NAME; // чтобы эффект появился в меню

} // epge ns
