#include <omp.h>
#include <cassert>
#include "glow.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "util/math/vec-util.hpp"
#include "game/util/locale.hpp"

namespace epge {

struct Glow::Impl {
  int _size {7};
  double _lightness {1.5};
  mutable int _glowmap_sz {};
  mutable Vector<real> _glowmap {};
  mutable real _glowmap_sum {};
  mutable Image _buffer {};

  inline Str name() const { return "glow"; }
  #define LOCSTR(NAME) get_locale_str("epge.effect.glow." NAME)
  inline utf32 localized_name() const { return LOCSTR("name"); }
  inline utf32 desc() const { return LOCSTR("desc"); }
  
  inline void draw(Image& dst) const {
    assert(dst);
    regen_glowmap();
    assert(!_glowmap.empty());
    assert(_glowmap_sum);
    _buffer = Image(dst);

    #pragma omp parallel for simd collapse(2)
    cfor (y, dst.Y)
    cfor (x, dst.X) {
      real total {};
      bool is_red = dst(x, y).is_red();

      cfor (gx, _glowmap_sz)
      cfor (gy, _glowmap_sz) {
        cauto light = _glowmap[gy * _glowmap_sz + gx];
        cauto src_pix = _buffer.get(
          x - _glowmap_sz/2 + gx,
          y - _glowmap_sz/2 + gy,
          Image_get::MIRROR
        );
        
        is_red |= src_pix.is_red();
        total += _lightness * light * src_pix.to_real();
      }
      
      dst(x, y) = blend_max(Pal8::from_real(total / _glowmap_sum, is_red), dst(x, y));
    }
  }

  inline void regen_glowmap() const {
    return_if (_glowmap_sz == _size);

    _glowmap_sz = _size;
    _glowmap_sum = 0;
    _glowmap.resize(_glowmap_sz * _glowmap_sz);
    cauto center = _glowmap_sz / 2.f ;
    assert(_glowmap_sz > 2);

    cfor (y, _glowmap_sz)
    cfor (x, _glowmap_sz) {
      cauto ratio = distance(Vec{x, y}, Vec{center, center} - 0.5f) / (center);
      cauto res = std::max<real>(0, 1.0 - ratio);
      _glowmap_sum += res;
      _glowmap[y * _glowmap_sz + x] = res;
    }
  }

  inline Params params() {
    return Params {
      new_shared<Param_int>("size", U"size", U"glow size", _size, 3, 50, 1, 2),
      new_shared<Param_double>("lightness", U"lightness", U"brightness of glow", _lightness, 0.1, 4, 0.1, 1),
    };
  }

  inline void update(const Delta_time dt) {}
}; // Impl

EPGE_IMPL_MAKER(Glow)

} // epge ns
