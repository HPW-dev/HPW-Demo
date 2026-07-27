#include "blur-helper.hpp"
#include "graphic/effect/blur.hpp"
#include "util/macro.hpp"
#include "game/core/graphic.hpp"

void hpw_blur(Image& dst, cr<Image> src, const int window_sz) {
  switch (graphic::blur_mode) {
    default:
    case Blur_mode::autoopt: {
      if (graphic::render_lag)
        blur_fast(dst, src, window_sz);
      else
        blur_hq(dst, src, window_sz);
      break;
    }
    case Blur_mode::low: blur_fast(dst, src, window_sz); break;
    case Blur_mode::high: blur_hq(dst, src, window_sz); break;
  }
}

bool check_high_blur() {
  switch (graphic::blur_mode) {
    default:
    case Blur_mode::autoopt: return !graphic::render_lag;
    case Blur_mode::low: return false;
    case Blur_mode::high: return true;
  }
  
  return false;
}
