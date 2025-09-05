#include <omp.h>
#include <cassert>
#include "epge.hpp"
#include "game/util/locale.hpp"
#include "graphic/image/image.hpp"

namespace epge {

#define LOCSTR(NAME) get_locale_str("epge.effect.fast_hblur." NAME)

class Fast_hblur: public epge::Base {
  int _window_sz = 2;
  int _mode = 1;

public:
  Str name() const { return "fast horizontal blur"; }
  utf32 localized_name() const { return LOCSTR("name"); }
  utf32 desc() const { return LOCSTR("desc"); }

  void draw_blur(Image& dst) const {
    cauto SZ = uint(_window_sz);
    cauto LINE = dst.size - SZ;

    #pragma omp parallel for
    cfor (i, LINE)
      cfor (wnd, SZ)
        dst[i] = blend_avr(dst[i + wnd + 1], dst[i]);
  }

  void draw_full_average(Image& dst) const {
    cauto SZ = uint(_window_sz);
    cauto LINE = dst.size - SZ;

    #pragma omp parallel for
    cfor (i, LINE) {
      cfor (wnd, SZ) {
        dst[i] = blend_avr(dst[i + wnd + 1], dst[i]);
        dst[i + wnd + 1] = dst[i];
      }
    }
  }
  
  void draw_bidir(Image& dst) const {
    #pragma omp parallel for
    for (int i = _window_sz; i < dst.size - _window_sz; ++i) {
      for (int wnd = -_window_sz; wnd < _window_sz + 1; ++wnd) {
        dst[i] = blend_avr(dst[i + wnd], dst[i]);
        dst[i + wnd] = dst[i];
      }
    }
  }  

  void draw_accurate(Image& dst) const {
    static Image buf;
    buf = dst;
    const real DIV = 1.0 / (_window_sz * 2 + 1);

    #pragma omp parallel for
    for (int i = _window_sz; i < dst.size - _window_sz; ++i) {
      real total = 0;

      for (int wnd = -_window_sz; wnd < _window_sz + 1; ++wnd)
        total += buf[i + wnd].to_real();
      
      dst[i] = Pal8::from_real(total * DIV);
    }
  }

  void draw(Image& dst) const {
    assert(dst);

    switch (_mode) {
      default:
      case 0: draw_blur(dst); break;
      case 1: draw_full_average(dst); break;
      case 2: draw_bidir(dst); break;
      case 3: draw_accurate(dst); break;
    }
  }

  Params params() {
    return Params {
      //new_shared<Param_int>(LOCSTR("param.power.name"), LOCSTR("param.power.desc"), _window_sz, 1, 16, 1, 2),
      new_shared<Param_int>("power", "", _window_sz, 1, 7, 1, 2),
      new_shared<Param_int>("mode", "0 - blur, 1 - full average, 2 - bidir, 3 - accurate", _mode, 0, 3, 1, 2),
    };
  }
}; // class

inline Epge_registrator<Fast_hblur> _ignore_Fast_hblur {};

} // epge ms
