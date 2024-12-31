#include <omp.h>
#include <cassert>
#include "flashes.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/effect/light.hpp"
#include "util/math/random.hpp"

namespace epge {

struct Flashes::Impl final {
  enum class Mode {
    full_star = 0,
    diagonal,
    MAX,
  };

  int _mode {scast<int>(Mode::full_star)};
  int _radius {7};
  double _chance {0.5};

  inline Str name() const noexcept { return "flashes"; }
  inline Str desc() const noexcept { return "draws flashes on bright pixels"; }

  inline void draw(Image& dst) const noexcept {
    assert(dst);
    // #pragma omp parallel for simd collapse(2) if (dst.size > 64 * 64)
  }

  inline epge::Params params() noexcept {
    return epge::Params {
      new_shared<epge::Param_int>("type", "type of flashes:\n  0 - full star, 1 - diagonal star",
        _mode, 0, scast<int>(Mode::MAX)-1, 1, 1),
      new_shared<epge::Param_int>("radius", "flash size", _radius, 1, 100, 1, 2),
      new_shared<epge::Param_double>("chance", "chance of a pixel flash", _chance, 0.01, 1, 0.01, 0.05),
    };
  }
}; // Impl

Flashes::Flashes(): impl{new_unique<Impl>()} {}
Flashes::~Flashes() {}
Str Flashes::name() const noexcept { return impl->name(); }
Str Flashes::desc() const noexcept { return impl->desc(); }
void Flashes::draw(Image& dst) const noexcept { impl->draw(dst); }
Params Flashes::params() noexcept { return impl->params(); }

} // epge ns
