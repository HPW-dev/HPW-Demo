#include <omp.h>
#include <cassert>
#include "video-noise.hpp"
#include "graphic/image/image.hpp"
#include "graphic/image/color-blend.hpp"
#include "util/math/random.hpp"

namespace epge {

struct Video_noise::Impl final {
  int _type {1};
  double _power {0.08};
  double _chance {0.7};

  inline Str name() const noexcept { return "noise"; }
  inline Str desc() const noexcept { return "add noise to every frame"; }
  
  inline void draw(Image& dst) const noexcept {
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

  inline Params params() noexcept {
    return Params {
      new_shared<Param_int>("type", "type of noise: 0-add, 1-add/sub, 2-sub, 3-average, 4-or, 5-xor", _type, 0, 5, 1, 2),
      new_shared<Param_double>("power", "power of noise", _power, 0.01, 1, 0.01, 0.1),
      new_shared<Param_double>("chance", "chance for applying noise for one pixel", _chance, 0.01, 1, 0.01, 0.1),
    };
  }
}; // Impl

EPGE_IMPL_MAKER(Video_noise)

} // epge ns
