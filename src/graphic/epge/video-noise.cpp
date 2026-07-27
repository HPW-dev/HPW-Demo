#include <omp.h>
#include <cassert>
#include "video-noise.hpp"
#include "graphic/image/image.hpp"
#include "graphic/image/color-blend.hpp"
#include "util/math/random.hpp"
#include "game/util/locale.hpp"

namespace epge {

struct Video_noise::Impl {
  int _type {1};
  double _power {0.08};
  double _chance {0.7};

  inline Str name() const { return "noise"; }
  #define LOCSTR(NAME) get_locale_str("epge.effect.noise." NAME)
  inline utf32 localized_name() const { return LOCSTR("name"); }
  inline utf32 desc() const { return LOCSTR("desc"); }
  
  inline void draw(Image& dst) const {
    assert(dst);

    #pragma omp parallel for
    for (auto& pix: dst) {
      cont_if (rndr_graphic() > _chance);
      switch (_type) {
        default:
        case 0: pix = blend_add_safe(Pal8::from_real(rndr_graphic(0, _power)), pix); break;
        case 1: {
          if (rndb_fast() % 2)
          pix = blend_sub_safe(Pal8::from_real(rndr_graphic(0, _power)), pix);
          else
          pix = blend_add_safe(Pal8::from_real(rndr_graphic(0, _power)), pix);
          break;
        }
        case 2: pix = blend_sub_safe(Pal8::from_real(rndr_graphic(0, _power)), pix); break;
        case 3: pix = blend_avr(Pal8::from_real(pix.to_real() + rndr_graphic(0, _power)), pix); break;
        case 4: pix = blend_or_safe(Pal8::from_real(rndr_graphic(0, _power)), pix); break;
        case 5: pix = blend_xor_safe(Pal8::from_real(rndr_graphic(0, _power)), pix); break;
      }
    }
  }

  inline Params params() {
    return Params {
      new_shared<Param_int>("type", U"type", U"type of noise: 0-add, 1-add/sub, 2-sub, 3-average, 4-or, 5-xor", _type, 0, 5, 1, 2),
      new_shared<Param_double>("power", U"power", U"power of noise", _power, 0.01, 1, 0.01, 0.1),
      new_shared<Param_double>("chance", U"chance", U"chance for applying noise for one pixel", _chance, 0.01, 1, 0.01, 0.1),
    };
  }

  inline void update(const Delta_time dt) {}
}; // Impl

EPGE_IMPL_MAKER(Video_noise)

} // epge ns
