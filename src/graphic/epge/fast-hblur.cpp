#include <omp.h>
#include <cassert>
#include "epge.hpp"
#include "game/util/locale.hpp"
#include "graphic/image/image.hpp"

namespace epge {

#define LOCSTR(NAME) get_locale_str("epge.effect.fast_hblur." NAME)

class Fast_hblur: public epge::Base {
  int _window_sz = 1;

public:
  Str name() const { return "fast horizontal blur"; }
  utf32 localized_name() const { return LOCSTR("name"); }
  utf32 desc() const { return LOCSTR("desc"); }

  void draw(Image& dst) const {
    assert(dst);
    cauto SZ = uint(_window_sz);
    cauto LINE = dst.size - SZ;

    #pragma omp parallel for
    cfor (i, LINE)
      cfor (wnd, SZ)
        dst[i] = blend_avr(dst[i + wnd + 1], dst[i]);
  }

  Params params() {
    return Params {
      //new_shared<Param_int>(LOCSTR("param.power.name"), LOCSTR("param.power.desc"), _window_sz, 1, 16, 1, 2),
      new_shared<Param_int>("power", "", _window_sz, 1, 7, 1, 2),
    };
  }
}; // class

inline Epge_registrator<Fast_hblur> _ignore_Fast_hblur {};

} // epge ms
