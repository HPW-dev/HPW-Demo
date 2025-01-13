#include <cassert>
#include <random>
#include <ctime>
#include "flashes-2.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/effect/light.hpp"

namespace epge {

struct Flashes_2::Impl final {
  enum class Mode {
    full_star = 0,
    diagonal,
    MAX,
  };

  int _mode {scast<int>(Mode::diagonal)};
  int _radius {1};
  double _chance {0.16};
  double _threshold {0.75};
  bool _halo {false};
  mutable std::default_random_engine _generator {};
  mutable std::uniform_real_distribution<double> _dist {0, 1};

  inline Str name() const noexcept { return "flashes v2"; }
  inline Str desc() const noexcept { return "draws flashes on bright pixels"; }

  inline Impl() { _generator.seed( std::time({}) ); }
  
  inline void draw(Image& dst) const noexcept {
    assert(dst);
    cauto is_diagonal = scast<Mode>(_mode) == Mode::diagonal ? true : false;

    cfor (y, dst.Y)
    cfor (x, dst.X) {
      cauto ratio = dst(x, y).to_real();
      continue_if (ratio < _threshold);
      continue_if (_dist(_generator) > _chance);
      Light lgt;
      lgt.radius = _radius * ratio;
      lgt.flags.random_radius = true;
      lgt.flags.repeat = true;
      lgt.flags.no_sphere = !_halo;
      lgt.flags.star = !is_diagonal;
      lgt.bf = &blend_max;
      lgt.bf_star = &blend_max;
      lgt.draw(dst, Vec(x, y));
    }
  }

  inline epge::Params params() noexcept {
    return epge::Params {
      new_shared<epge::Param_int>("type", "type of flashes:\n  0 - full star, 1 - diagonal star",
        _mode, 0, scast<int>(Mode::MAX)-1, 1, 1),
      new_shared<epge::Param_int>("radius", "flash size", _radius, 1, 100, 1, 2),
      new_shared<epge::Param_double>("chance", "chance of a pixel flash", _chance, 0.01, 1, 0.01, 0.05),
      new_shared<epge::Param_double>("threadid", "brightness threshold for flash to occur", _threshold, 0, 1, 0.05, 0.1),
      new_shared<epge::Param_bool>("halo", "display flash halo", _halo),
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
