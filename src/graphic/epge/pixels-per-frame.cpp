#include <omp.h>
#include "pixels-per-frame.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/util/locale.hpp"

namespace epge {

struct Pixels_per_frame::Impl {
  mutable Image _buffer {};
  mutable int _cur_pos {}; // позиция текущего рисуемого пикселя
  int _ppf {512 * 211};

  inline Str name() const { return "pixels per frame"; }
  #define LOCSTR(NAME) get_locale_str("epge.effect.pixels_per_frame." NAME)
  inline utf32 localized_name() const { return LOCSTR("name"); }
  inline utf32 desc() const { return LOCSTR("desc"); }

  inline void draw(Image& dst) const {
    assert(dst);
    assert(_ppf > 0);

    _buffer = dst;
    dst.fill(Pal8::black);
    cauto dst_size = dst.size;
    
    #pragma omp parallel for simd
    cfor (i, _ppf) {
      cauto idx = (_cur_pos + i) % dst_size;
      dst[idx] = _buffer[idx];
    }

    _cur_pos += _ppf;
    _cur_pos %= dst_size;
  }

  inline epge::Params params() {
    return epge::Params {
      new_shared<epge::Param_int>("pixels per frame", "number of pixels per frame", _ppf, 1, 512*384 - 1, 100, 1000),
    };
  }
}; // Impl

EPGE_IMPL_MAKER(Pixels_per_frame)

} // epge ns
