#include <omp.h>
#include <cassert>
#include <algorithm>
#include "bgp-pack-4.hpp"
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
  enum class Impact {
    none,
    elastic,
    inelastic,
  };

  Bound bound {}; // режим рамки
  Figure figure {}; // тип фигуры
  Colors colors {}; // как раскрашивать фигуры
  Impact impact {}; // тип удара
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

  inline void bound_pocess(Object_base& obj) {
    if (_cfg.bound == Config::Bound::screen) {
      if (obj.pos.x < 0) {
        obj.pos.x = 0;
        obj.v.x *= -1;
      }
      if (obj.pos.x >= graphic::width) {
        obj.pos.x = graphic::width - 1;
        obj.v.x *= -1;
      }

      if (obj.pos.y < 0) {
        obj.pos.y = 0;
        obj.v.y *= -1;
      }
      if (obj.pos.y >= graphic::height) {
        obj.pos.y = graphic::height - 1;
        obj.v.y *= -1;
      }
    } else {
      error("need impl");
    }
  }

  inline void ellastic_impact(Object_base& a, Object_base& b) const {
    cauto av = a.v;
    cauto bv = b.v;
    cauto abm = a.m + b.m;
    a.v = (av * (a.m - b.m) - bv * 2 * b.m) / abm;
    b.v = (bv * (b.m - a.m) + av * 2 * a.m) / abm;
  }

  inline void inelastic_impact(Object_base& a, Object_base& b) const {
    a.v = b.v = (a.v * a.m + b.v * b.m) / (a.m + b.m);
  }

  inline void impact_process() {
    ret_if(_cfg.impact == Config::Impact::none);

    cauto figures_sz = _figures.size();
    assert(figures_sz > 1);

    for (std::size_t a_i = 0; a_i < figures_sz - 1; ++a_i) {
      assert(_figures[a_i]);
      rauto a = *_figures[a_i];

      for (std::size_t b_i = a_i + 1; b_i < figures_sz; ++b_i) {
        assert(_figures[b_i]);
        rauto b = *_figures[b_i];

        cont_if(std::addressof(a) == std::addressof(b));

        if (a.check_collision(b))
        switch (_cfg.impact) {
          case Config::Impact::elastic: ellastic_impact(a, b); break;
          case Config::Impact::inelastic: inelastic_impact(a, b); break;
          default: error("unknown impact type");
        }
      }
    }
  }

  inline void update(Delta_time dt) {
    for (rauto fig: _figures) {
      assert(fig);
      fig->update(dt);
      bound_pocess(*fig);
    }

    impact_process();

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

  static uint state = 0;
  static Config cfg {
    .bound = Config::Bound::screen,
    .figure = Config::Figure::circle,
    .colors = Config::Colors::red,
    .impact = Config::Impact::elastic,
    .gravity = false,
    .gravity_power = 0,
    .pushing_out = false,
    .fill_color = false,
    .mass_min = 10,
    .mass_max = 40,
    .size_min = 3,
    .size_max = 11,
    .speed_min = 0.2_pps,
    .speed_max = 6.0_pps,
    .count_min = 70,
    .count_max = 120,
    .scale = 1,
    .bg_color = Pal8::black,
    .seed = 97'997
  };
  static Physics_simulation sim(cfg);

  sim.update(1.0 / 60.0);
  sim.draw(dst);

  if (state++ % 433 == 0) {
    cfg.seed += state;
    sim.init(cfg);
  }
}

void bgp_physics_2(Image& dst, const int bg_state) {
  assert(dst);

  static uint state = 0;
  static Config cfg {
    .bound = Config::Bound::screen,
    .figure = Config::Figure::circle,
    .colors = Config::Colors::white,
    .impact = Config::Impact::inelastic,
    .gravity = false,
    .gravity_power = 0,
    .pushing_out = false,
    .fill_color = true,
    .mass_min = 1,
    .mass_max = 4,
    .size_min = 1,
    .size_max = 7,
    .speed_min = 1.0_pps,
    .speed_max = 10.0_pps,
    .count_min = 200,
    .count_max = 400,
    .scale = 1,
    .bg_color = Pal8::black,
    .seed = 97'997
  };
  static Physics_simulation sim(cfg);

  sim.update(1.0 / 60.0);
  sim.draw(dst);

  if (state++ % 300 == 0) {
    cfg.seed += state;
    sim.init(cfg);
  }
}
