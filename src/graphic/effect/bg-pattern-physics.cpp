#include <omp.h>
#include <cassert>
#include <algorithm>
#include "bg-pattern-physics.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/resize.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/math/circle.hpp"
#include "util/math/rect.hpp"
#include "util/math/xorshift.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/mat.hpp"
#include "util/str-util.hpp"
#include "util/mem-types.hpp"
#include "util/error.hpp"
#include "util/hpw-util.hpp"
#include "game/core/canvas.hpp"

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
  double gravity_power {}; // как быстро объекты притягиваются к друг другу
  bool pushing_out {}; // выталкивать объект, если он застрял внутри другого
  bool fill_color {}; // заливать цветом
  double mass_min {};
  double mass_max {};
  double size_min {};
  double size_max {};
  double speed_min {};
  double speed_max {};
  uint count_min {};
  uint count_max {};
  uint scale {}; // во сколько раз увеличить пиксели
  Pal8 bg_color {};
  std::uint32_t seed {};
}; // Config

struct Object_base {
  double m {}; // масса объекта
  Vecd pos {};
  Vecd v {}; // вектор скорости
  Pal8 color {};
  bool filled {};

  inline explicit Object_base(const Pal8 _color, double mass, cr<Vecd> _pos, cr<Vecd> velocity, bool _filled)
  : m {mass}
  , pos {_pos}
  , v {velocity}
  , color {_color}
  , filled {_filled}
  { assert(m > 0); }

  virtual ~Object_base() = default;

  void update(Delta_time dt) { pos += v * dt; }
  virtual bool check_collision(cr<Object_base> other) const = 0;
  virtual void draw(Image& dst) const = 0;
};

struct Object_circle: public Object_base {
  double r {};

  inline explicit Object_circle(const Pal8 _color, double mass, cr<Vecd> _pos,
  cr<Vecd> velocity, double radius, bool _filled)
  : Object_base(_color, mass, _pos, velocity, _filled), r{radius}
  { assert(r > 0); }

  inline bool check_collision(cr<Object_base> other) const override { 
    Circle a;
    a.r = r;

    crauto other_circle = rcast<cr<Object_circle>>(other);
    Circle b;
    b.r = other_circle.r;

    return a.is_collided(this->pos, other_circle.pos, b);
  }

  inline void draw(Image& dst) const override {
    if (filled)
      draw_circle_filled(dst, pos, r, color);
    else
      draw_circle(dst, pos, r, color);
  }
}; // Object_circle

struct Object_square: public Object_base {
  double sz {};

  inline explicit Object_square(const Pal8 _color, double mass, cr<Vecd> _pos,
  cr<Vecd> velocity, double _sz, bool _filled)
  : Object_base(_color, mass, _pos, velocity, _filled), sz {_sz}
  { assert(sz > 0); }

  inline bool check_collision(cr<Object_base> other) const override {
    const double this_half = sz / 2.0;
    const Rectd a(this->pos - this_half, Vecd{sz, sz});

    crauto other_square = rcast<cr<Object_square>>(other);
    const double other_half = other_square.sz / 2.0;
    const Rectd b(other_square.pos - other_half, Vecd{other_square.sz, other_square.sz});

    return intersect(a, b);
  }

  inline void draw(Image& dst) const override {
    const double this_half = sz / 2.0;
    const Rectd rect(this->pos - this_half, Vecd{sz, sz});

    if (filled)
      draw_rect_filled(dst, rect, color);
    else
      draw_rect(dst, rect, color);
  }
}; // Object_square

struct Physics_simulation {
  Config _cfg {};
  using Figures = Vector<Unique<Object_base>>;
  Figures _figures {};
  xorshift128_state _seed {};

  explicit inline Physics_simulation(cr<Config> cfg) { init(cfg); }

  inline uint rndd(double a=0, double b=1) {
    return_if (a == b, a);
    assert(a < b);
    return a + (b - a) * (std::fmod<double>(xorshift128(_seed), 100'000.0) / 100'000.0);
  }

  inline uint rndu(uint a, uint b) {
    return_if (a == b, a);
    assert(a < b);
    return a + xorshift128(_seed) % (b - a + 1);
  }

  inline Pal8 get_color() {
    switch (_cfg.colors) {
      case Config::Colors::white: return Pal8::white;
      case Config::Colors::red: return Pal8::red;
      default: error("unknown color type");
    }

    return Pal8::white;
  }

  inline Vecd get_pos() {
    if (_cfg.bound == Config::Bound::circle) {
      error("need impl");
    } elif (_cfg.bound == Config::Bound::screen) {
      return Vecd(
        rndd(0, graphic::width),
        rndd(0, graphic::height)
      );
    } else {
      error("unknown bound type");
    }

    return {};
  }

  inline Vecd get_vel() {
    cauto speed = rndd(_cfg.speed_min, _cfg.speed_max);
    return deg_to_vec<double>(rndd(0, 360)) * speed;
  }

  inline void init(cr<Config> cfg) {
    _cfg = cfg;

    assert(_cfg.mass_min > 0);
    assert(_cfg.mass_max >= _cfg.mass_min);
    assert(_cfg.size_min > 0);
    assert(_cfg.size_max >= _cfg.size_min);
    assert(_cfg.speed_min >= 0);
    assert(_cfg.speed_max >= _cfg.speed_min);
    assert(_cfg.count_min > 0);
    assert(_cfg.count_max >= _cfg.count_min);
    assert(_cfg.scale > 0);
    assert(_cfg.scale < 9);
    assert(_cfg.seed > 1);

    _seed.a = 123'456 ^ _cfg.seed;
    _seed.b = 789'012 ^ _cfg.seed;
    _seed.c = 345'543 ^ _cfg.seed;
    _seed.d = 979'977 ^ _cfg.seed;

    generate_figures();
  }

  inline void generate_figures() {
    _figures.clear();
    cauto figures_sz = rndu(_cfg.count_min, _cfg.count_max);
    _figures.resize(figures_sz);

    for (rauto fig: _figures) {
      cauto mass = rndd(_cfg.mass_min, _cfg.mass_max);
      cauto mass_ratio = mass / scast<double>(_cfg.mass_max);

      if (_cfg.figure == Config::Figure::circle)
        fig = new_unique<Object_circle>(
          get_color(),
          mass,
          get_pos(),
          get_vel(),
          std::lerp(_cfg.size_min, _cfg.size_max, mass_ratio),
          _cfg.fill_color  
        );
      elif (_cfg.figure == Config::Figure::square)
        fig = new_unique<Object_square>(
          get_color(),
          mass,
          get_pos(),
          get_vel(),
          std::lerp(_cfg.size_min, _cfg.size_max, mass_ratio),
          _cfg.fill_color  
        );
      else
        error("unknown figure");
    }
  }

  inline void update(Delta_time dt) {
    for (rauto fig: _figures) {
      assert(fig);
      fig->update(dt);
    }

    iferror(_cfg.elastic_impact == false, "need impl");
    iferror(_cfg.gravity, "need impl");
    iferror(_cfg.gravity_power != 0, "need impl");
    iferror(_cfg.pushing_out, "need impl");
  }

  inline void draw(Image& dst) const {
    dst.fill(_cfg.bg_color);

    for (rauto fig: _figures) {
      assert(fig);
      fig->draw(dst);
    }

    iferror(_cfg.scale != 1, "need impl for resizes");
  }
}; // Physics_simulation

void bgp_physics_1(Image& dst, const int bg_state) {
  assert(dst);

  static const Config cfg {
    .bound = Config::Bound::screen,
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
    .size_max = 15,
    .speed_min = 0.2_pps,
    .speed_max = 4.0_pps,
    .count_min = 40,
    .count_max = 70,
    .scale = 1,
    .bg_color = Pal8::black,
    .seed = 97'997
  };
  static Physics_simulation sim(cfg);

  sim.update(1.0 / 60.0);
  sim.draw(dst);
}
