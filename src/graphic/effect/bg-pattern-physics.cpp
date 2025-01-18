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
  uint count_min {};
  uint count_max {};
  uint scale {}; // во сколько раз увеличить пиксели
  Pal8 bg_color {};
  std::uint32_t seed {};
}; // Config

struct Object_base {
  Pal8 color {};
  real m {}; // масса объекта
  Vecd pos {};
  Vecd v {}; // вектор скорости

  inline explicit Object_base(const Pal8 _color, real mass, cr<Vecd> _pos, cr<Vecd> velocity)
  : color {_color}
  , m {mass}
  , pos {_pos}
  , v {velocity}
  { assert(m > 0); }

  virtual ~Object_base() = default;

  void update(Delta_time dt) { pos += v * dt; }
  virtual bool check_collision(cr<Object_base> other) const = 0;
  virtual void draw(Image dst) const = 0;
};

struct Object_circle: public Object_base {
  real r {};

  inline explicit Object_circle(const Pal8 _color, real mass, cr<Vecd> _pos, cr<Vecd> velocity, real radius)
  : Object_base(_color, mass, _pos, velocity), r{radius}
  { assert(r > 0); }

  inline bool check_collision(cr<Object_base> other) const override { 
    // TODO
    return false;
  }

  inline void draw(Image dst) const override {
    // TODO
  }
}; // Object_circle

struct Object_square: public Object_base {
  real sz {};

  inline explicit Object_square(const Pal8 _color, real mass, cr<Vecd> _pos, cr<Vecd> velocity, real _sz)
  : Object_base(_color, mass, _pos, velocity), sz {_sz}
  { assert(sz > 0); }

  inline bool check_collision(cr<Object_base> other) const override { 
    // TODO
    return false;
  }

  inline void draw(Image dst) const override {
    // TODO
  }
}; // Object_square

struct Physics_simulation {
  Config _cfg {};

  explicit inline Physics_simulation(cr<Config> cfg) { init(cfg); }

  inline void init(cr<Config> cfg) {
    _cfg = cfg;
    assert(_cfg.mass_min > 0);
    assert(_cfg.mass_max >= _cfg.mass_min);
    assert(_cfg.size_min > 0);
    assert(_cfg.size_max >= _cfg.size_min);
    assert(_cfg.count_min > 0);
    assert(_cfg.count_max >= _cfg.count_min);
    assert(_cfg.scale > 0);
    assert(_cfg.scale < 9);
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
    .count_min = 4,
    .count_max = 7,
    .scale = 1,
    .bg_color = Pal8::black,
    .seed = 97'997,
  };
  static Physics_simulation sim(cfg);

  sim.update(1.0 / 60.0);
  sim.draw(dst);
}
