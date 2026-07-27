#include <omp.h>
#include <cassert>
#include <cmath>
#include "epge.hpp"
#include "game/util/locale.hpp"
#include "util/math/random.hpp"
#include "graphic/image/image.hpp"

EPGE_CLASS_BEGIN(stretch)
  int _shift = 2;
  int _mode = 0;
  bool _random = false;
  mutable Image _buf {};

public:
  void h_stretch(Image& dst) const {
    _buf = dst;

    cauto SHIFT = _random
      ? (rndu_fast(std::abs(_shift)) * (rndb_fast()%2 ? -1 : 1))
      : _shift;

    #pragma omp parallel for
    cfor (y, dst.Y) {
      if (y % 2)
        cfor (x, dst.X)
          dst.set(x + SHIFT, y, _buf.fast_get(x, y), {});
    }
  }
  
  void v_stretch(Image& dst) const {
    _buf = dst;
    
    cauto SHIFT = _random
      ? (rndu_fast(std::abs(_shift)) * (rndb_fast()%2 ? -1 : 1))
      : _shift;

    #pragma omp parallel for
    cfor (x, dst.X) {
      if (x % 2)
        cfor (y, dst.Y)
          dst.set(x, y + SHIFT, _buf.fast_get(x, y), {});
    }
  }

  void draw(Image& dst) const {
    ret_if(_shift == 0);
    assert(dst);

    switch (_mode) {
      default:
      case 0: h_stretch(dst); break;
      case 1: v_stretch(dst); break;
    }
  }

  Params params() {
    return Params {
      EPGE_PARAM_INT(shift, stretch, _shift, -32, 32, 1, 2)
      EPGE_PARAM_INT(mode, stretch, _mode, 0, 1, 1, 1)
      EPGE_PARAM_BOOL(random, stretch, _random)
    };
  }
EPGE_CLASS_END(stretch)
