#include <omp.h>
#include <cassert>
#include <cmath>
#include "epge.hpp"
#include "game/util/locale.hpp"
#include "util/math/random.hpp"
#include "graphic/image/image.hpp"

namespace epge {

#define EFFECT_NAME stretch
#define CLASS_NAME CONCAT(Epge_, EFFECT_NAME)
#define REGISTRATOR_NAME CONCAT(_registrator_for_, EFFECT_NAME)
#define EFFECT_NAME_STR STRINGIFY(EFFECT_NAME)
#define LOCSTR(NAME) get_locale_str("epge.effect." EFFECT_NAME_STR "." NAME)

class CLASS_NAME: public epge::Base {
  int _shift = 2;
  int _mode = 0;
  bool _random = false;
  mutable Image _buf {};

public:
  Str name() const { return EFFECT_NAME_STR; }
  utf32 localized_name() const { return LOCSTR("name"); }
  utf32 desc() const { return LOCSTR("desc"); }

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
      new_shared<Param_int>("shift", LOCSTR("param.shift.name"), LOCSTR("param.shift.desc"), _shift, -32, 32, 1, 2),
      new_shared<Param_int>("mode", LOCSTR("param.mode.name"), LOCSTR("param.mode.desc"), _mode, 0, 1, 1, 1),
      new_shared<Param_bool>("random", LOCSTR("param.random.name"), LOCSTR("param.random.desc"), _random),
    };
  }
}; // class

inline Epge_registrator<CLASS_NAME> REGISTRATOR_NAME; // чтобы эффект появился в меню

} // epge ms
