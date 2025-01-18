#include <omp.h>
#include <cassert>
#include <algorithm>
#include "bg-pattern-physics.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/resize.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/math/xorshift.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/mat.hpp"
#include "util/str-util.hpp"

struct Config {
  enum class Bound {
    circle,
    screen,
  };
  enum class Figure {
    circle,
    square,
  };
  enum class Colors {
    none,
    red,
    white,
    grad_gray,
    grad_red,
    random,
  };

  Bound bound {}; // режим рамки
  Figure figure {}; // тип фигуры
  Colors colors {}; // как раскрашивать фигуры
  bool elastic_impact {}; // упругий удар
  bool gravity {}; // притяжение объектов друг к другу
  real gravity_power {}; // как быстро объекты притягиваются к друг другу
  bool pushing_out {}; // выталкивать объект, если он застрял внутри другого
  bool fill_color {}; // заливать цветом
  real mass_min {};
  real mass_max {};
  real size_min {};
  real size_max {};
  Pal8 bg_color {};
  std::uint32_t seed {};
}; // Config

struct Physics_simulation {
  Config _cfg {};

  explicit inline Physics_simulation(cr<Config> cfg) { init(cfg); }

  inline void init(cr<Config> cfg) {
    _cfg = cfg;
    assert(_cfg.mass_min > 0);
    assert(_cfg.mass_max >= _cfg.mass_min);
    assert(_cfg.size_min > 0);
    assert(_cfg.size_max >= _cfg.size_min);
    assert(_cfg.seed > 1);
  }

  inline void update(Delta_time dt) {

  }

  inline void draw(Image& dst) const {
    dst.fill(_cfg.bg_color);
  }
}; // Physics_simulation

void bgp_physics_1(Image& dst, const int bg_state) {
  assert(dst);

  static const Config cfg {
    .bound = Config::Bound::circle,
    .figure = Config::Figure::circle,
    .colors = Config::Colors::white,
    .elastic_impact = true,
    .gravity = false,
    .gravity_power = 0,
    .pushing_out = false,
    .fill_color = false,
    .mass_min = 1,
    .mass_max = 4,
    .size_min = 5,
    .size_max = 9,
    .bg_color = Pal8::black,
    .seed = 97'997,
  };
  static Physics_simulation sim(cfg);

  sim.update(1.0 / 60.0);
  sim.draw(dst);
}
