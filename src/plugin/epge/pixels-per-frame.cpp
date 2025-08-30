#include <omp.h>
#include "pixels-per-frame.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"

namespace epge {

struct Pixels_per_frame::Impl final {
  mutable Image _buffer {};
  mutable int _cur_pos {}; // позиция текущего рисуемого пикселя
  int _ppf {512 * 211};

  inline Str name() const noexcept { return "pixels per frame"; }
  inline Str desc() const noexcept { return "draws a certain number of pixels per frame"; }

  inline void draw(Image& dst) const noexcept {
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

  inline epge::Params params() noexcept {
    return epge::Params {
      new_shared<epge::Param_int>("pixels per frame", "number of pixels per frame", _ppf, 1, 512*384 - 1, 100, 1000),
    };
  }
}; // Impl

EPGE_IMPL_MAKER(Pixels_per_frame)

} // epge ns
