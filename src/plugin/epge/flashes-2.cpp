#include <omp.h>
#include <cassert>
#include "flashes-2.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "util/math/vec-util.hpp"

namespace epge {

struct Flashes_2::Impl final {
  int _size {5};
  double _lightness {0.5};
  mutable int _glowmap_sz {};
  mutable Vector<real> _glowmap {};

  inline Str name() const noexcept { return "flashes v2"; }
  inline Str desc() const noexcept { return "soft glow"; }
  
  inline void draw(Image& dst) const noexcept {
    assert(dst);
    regen_glowmap();

    cfor (y, _glowmap_sz)
    cfor (x, _glowmap_sz) {
      dst(x, y) = Pal8::from_real(_glowmap[y * _glowmap_sz + x]);
    }
  }

  inline void regen_glowmap() const {
    return_if (_glowmap_sz == _size);

    _glowmap_sz = _size;
    _glowmap = Vector<real>(_glowmap_sz * _glowmap_sz);
    cauto center = _glowmap_sz / 2.f ;
    assert(_glowmap_sz > 2);

    cfor (y, _glowmap_sz)
    cfor (x, _glowmap_sz) {
      cauto ratio = distance(Vec{x, y}, Vec{center, center} - 0.5f) / (center);
      _glowmap[y * _glowmap_sz + x] = std::max<real>(0, 1.0 - ratio);
    }
  }

  inline Params params() noexcept {
    return Params {
      new_shared<Param_int>("size", "glow size", _size, 3, 32, 1, 2),
      new_shared<Param_double>("lightness", "brightness of glow", _lightness, 0.01, 1, 0.01, 0.05),
    };
  }
}; // Impl

Flashes_2::Flashes_2(): impl{new_unique<Impl>()} {}
Flashes_2::~Flashes_2() {}
Str Flashes_2::name() const noexcept { return impl->name(); }
Str Flashes_2::desc() const noexcept { return impl->desc(); }
void Flashes_2::draw(Image& dst) const noexcept { impl->draw(dst); }
Params Flashes_2::params() noexcept { return impl->params(); }

} // epge ns
