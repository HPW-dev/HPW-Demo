#include <cassert>
#include <random>
#include <ctime>
#include "flashes.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/effect/light.hpp"
#include "game/util/locale.hpp"

namespace epge {

struct Flashes::Impl {
  enum class Mode {
    full_star = 0,
    diagonal,
    MAX,
  };

  int _mode {scast<int>(Mode::full_star)};
  int _radius {16};
  double _chance {0.1};
  double _threshold {0.75};
  bool _halo {false};
  mutable std::default_random_engine _generator {};
  mutable std::uniform_real_distribution<double> _dist {0, 1};

  inline Str name() const { return "flashes"; }
  #define LOCSTR(NAME) get_locale_str("epge.effect.flashes." NAME)
  inline utf32 localized_name() const { return LOCSTR("name"); }
  inline utf32 desc() const { return LOCSTR("desc"); }

  inline Impl() { _generator.seed( std::time({}) ); }
  
  inline void draw(Image& dst) const {
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

  inline epge::Params params() {
    return epge::Params {
      new_shared<epge::Param_int>("type", U"type", U"type of flashes:\n  0 - full star, 1 - diagonal star",
        _mode, 0, scast<int>(Mode::MAX)-1, 1, 1),
      new_shared<epge::Param_int>("radius", U"radius", U"flash size", _radius, 1, 100, 1, 2),
      new_shared<epge::Param_double>("chance", U"chance", U"chance of a pixel flash", _chance, 0.01, 1, 0.01, 0.05),
      new_shared<epge::Param_double>("threadid", U"threadid", U"brightness threshold for flash to occur", _threshold, 0, 1, 0.05, 0.1),
      new_shared<epge::Param_bool>("halo", U"halo", U"display flash halo", _halo),
    };
  }
}; // Impl

EPGE_IMPL_MAKER(Flashes)

} // epge ns
