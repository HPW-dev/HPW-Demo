#include <omp.h>
#include "scanline.hpp"
#include "graphic/image/color-blend.hpp"
#include "graphic/image/image.hpp"
#include "game/util/locale.hpp"

namespace epge {

struct Scanline::Impl {
  double _intense {0.25}; // степень затенения полосками
  int _type {0}; // тип полосок: 0 - горизонтальные, 1 - верт-е, 2 - сетка
  bool _shading {true}; // true - уменьшать яркость в полосках. Иначе - увеличивать

  inline Str name() const { return "scanline"; }
  #define LOCSTR(NAME) get_locale_str("epge.effect.scanline." NAME)
  inline utf32 localized_name() const { return LOCSTR("name"); }
  inline utf32 desc() const { return LOCSTR("desc"); }

  inline void draw(Image& dst) const {
    cauto SCANLINE_SHADOW = Pal8::from_real(_intense);
    cauto BLEND_F = _shading ? blend_sub_safe : blend_add_safe;

    switch (_type) {
      default:
      case 0: { // hline
        #pragma omp parallel for simd if (dst.size > 64 * 64)
        cfor (y, dst.Y)
          if (y % 2)
            cfor (x, dst.X)
              dst(x, y) = BLEND_F(SCANLINE_SHADOW, dst(x, y), {});
        break;
      }

      case 1: { // vline
        #pragma omp parallel for simd collapse(2) if (dst.size > 64 * 64)
        cfor (y, dst.Y)
        cfor (x, dst.X)
          if (x % 2)
            dst(x, y) = BLEND_F(SCANLINE_SHADOW, dst(x, y), {});
        break;
      }

      case 2: { // grid
        #pragma omp parallel for simd collapse(2) if (dst.size > 64 * 64)
        cfor (y, dst.Y)
        cfor (x, dst.X)
          if ((x % 2) || (y % 2))
            dst(x, y) = BLEND_F(SCANLINE_SHADOW, dst(x, y), {});
        break;
      }
    } // switch _type
  }

  inline epge::Params params() {
    return epge::Params {
      new_shared<epge::Param_double>("intense", U"intense", U"degree of shading/lighting by lines", _intense, 0.005, 1, 0.005, 0.01),
      new_shared<epge::Param_bool>("shading", U"shading", U"reduce brightness in lines", _shading),
      new_shared<epge::Param_int>("line_type", U"line type", U"type of scanlines:\n0 - horizontal, 1 - vertical, 2 - grid",
        _type, 0, 2, 1, 1),
    };
  }

  inline void update(const Delta_time dt) {}
}; // Impl

EPGE_IMPL_MAKER(Scanline)

} // epge ns
