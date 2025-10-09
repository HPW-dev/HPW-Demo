#include <omp.h>
#include <cassert>
#include "fading.hpp"
#include "graphic/image/color-blend.hpp"
#include "graphic/image/image.hpp"
#include "game/util/locale.hpp"

namespace epge {

struct Fading::Impl {
  enum class Mode {
    average = 0,
    maximum,
    minimum,
    MAX
  };

  double _power {0.8};
  int _mode {scast<int>(Mode::maximum)};
  mutable Image buffer {};

  inline Str name() const { return "fading"; }
  #define LOCSTR(NAME) get_locale_str("epge.effect.fading." NAME)
  inline utf32 localized_name() const { return LOCSTR("name"); }
  inline utf32 desc() const { return LOCSTR("desc"); }

  inline void draw(Image& dst) const {
    assert(dst);
    if (buffer.size != dst.size)
      buffer = dst;

    switch (scast<Mode>(_mode)) {
      default:
      case Mode::average: average_fading(dst); break;
      case Mode::maximum: maximum_fading(dst); break;
      case Mode::minimum: minimum_fading(dst); break;
    }
  }

  inline void average_fading(Image& dst) const {
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size) {
      auto a = buffer[i].to_real();
      cauto b = dst[i].to_real();
      const bool is_red = dst[i].is_red();
      a = (a * _power) + (b * (1.0f - _power));
      dst[i] = Pal8::from_real(a, is_red);
      buffer[i] = dst[i];
    }
  }

  inline void maximum_fading(Image& dst) const {
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size) {
      auto a = buffer[i].to_real();
      cauto b = dst[i].to_real();
      const bool is_red = dst[i].is_red();
      a = std::max(a, b);
      dst[i] = Pal8::from_real(a, is_red);
      buffer[i] = Pal8::from_real(a - (1.f - _power), is_red);
    }
  }

  inline void minimum_fading(Image& dst) const {
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size) {
      auto a = buffer[i].to_real();
      cauto b = dst[i].to_real();
      const bool is_red = dst[i].is_red();
      a = std::min(a, b);
      dst[i] = Pal8::from_real(a, is_red);
      buffer[i] = Pal8::from_real(a + (1.f - _power), is_red);
    }
  }

  inline epge::Params params() {
    return epge::Params {
      new_shared<epge::Param_double>("power", U"power", U"0.99 - slow fading, 0.01 - fast", _power, 0.01, 0.99, 0.01, 0.03),
      new_shared<epge::Param_int>("mode", U"mode", U"pixel blending modes:\n  0 - average, 1 - max, 2 - min", _mode, 0, scast<int>(Mode::MAX)-1, 1, 1),
    };
  }

  inline void update(const Delta_time dt) {}
}; // Impl

EPGE_IMPL_MAKER(Fading)

} // epge ns
