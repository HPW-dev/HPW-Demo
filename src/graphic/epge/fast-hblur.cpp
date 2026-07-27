#include <omp.h>
#include <cassert>
#include "epge.hpp"
#include "game/util/locale.hpp"
#include "graphic/image/image.hpp"

EPGE_CLASS_BEGIN(fast_hblur)
  int _window_sz = 2;
  int _mode = 1;

public:
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
      EPGE_PARAM_INT(power, fast_hblur, _window_sz, 1, 7, 1, 2)
      EPGE_PARAM_INT(mode, fast_hblur, _mode, 0, 3, 1, 2)
    };
  }
EPGE_CLASS_END(fast_hblur)
