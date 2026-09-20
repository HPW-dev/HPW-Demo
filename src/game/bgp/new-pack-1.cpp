#include "pch.hpp"
#include "bgp.hpp"
#include "game/core/fonts.hpp"
#include "engine/graphic/image/image.hpp"
#include "engine/graphic/util/graphic-util.hpp"
#include "engine/graphic/util/util-templ.hpp"

namespace bgp {
// ---------------------------- Код фонов --------------------------------------

// просто заливка
class Simple_color: public Bgp {
  Pal8 _color {};
public:
  inline Simple_color() { restart(); }
  inline void restart() override { _color = rand_color_graphic(); }
  inline void draw(Image& dst) const override { dst.fill(_color); }
};

// просто заливка
class Empty: public Bgp { public: inline void draw(Image& _) const override {} };

namespace Repeated_circles {
struct Context {
  Vec p0 {100, 160};
  Vec p1 {450, 200};
  Pal8 bg {};
  Pal8 fg {Pal8::white};
  uint count = 250;
  real step {2};
  bool blink {true};
  bool rnd_colors {false};
  bool rnd_step {false};
};
inline static void draw(Image& dst, const Context& ctx) {
  dst.fill(ctx.bg);
  real r0 = 1;
  real r1 = 1;
  for (uint _ = 0; _ < ctx.count; ++_) {
    Pal8 c0 = ctx.rnd_colors ? Pal8::from_real(rndr_fast()) : ctx.fg;
    Pal8 c1 = ctx.rnd_colors ? Pal8::from_real(rndr_fast()) : ctx.fg;
    if (ctx.blink ? rndb_fast()%2 : true) draw_circle(dst, ctx.p0, r0, c0);
    if (ctx.blink ? rndb_fast()%2 : true) draw_circle(dst, ctx.p1, r1, c1);
    r0 += ctx.rnd_step ? rndr_fast(0, ctx.step) : ctx.step;
    r1 += ctx.rnd_step ? rndr_fast(0, ctx.step) : ctx.step;
  }
}
} // Repeated_circles ns

// Белые пульсирующие кольца
class Repeated_circles_0: public Bgp {
  inline void draw(Image& dst) const override {
    Repeated_circles::Context ctx {
      .p0 = {100, 160},
      .p1 = {450, 200},
      .bg = Pal8::black,
      .fg = {Pal8::white},
      .count = 250,
      .step = 2,
      .blink = true,
      .rnd_colors = false,
      .rnd_step = false,
    };
    Repeated_circles::draw(dst, ctx);
  }
};

// Диски на красном фоне что шлифуют глаза
class Repeated_circles_1: public Bgp {
  inline void draw(Image& dst) const override {
    Repeated_circles::Context ctx {
      .p0 = {0, 384},
      .p1 = {512, 0},
      .bg = Pal8::red,
      .fg = {Pal8::white},
      .count = 550,
      .step = 1.5,
      .blink = false,
      .rnd_colors = true,
      .rnd_step = true,
    };
    Repeated_circles::draw(dst, ctx);
  }
};

// Настройка эффекта червячков
struct Asci_worms_context {
  uint screen_w {512};
  uint screen_h {384};
  uint map_w {512/8 + 1};
  uint map_h {384/18 + 2};
  Veci start_pos {(512/8)/2, (384/18)/2};
  uint count {3};
  Font_base* font {graphic::font.get()};
};

// Блуждающие по буквам червяки, которые переворачивают буквы
class Asci_worms: public Bgp {
  Asci_worms_context _ctx {};
  Vector<Veci> _worms {};
  Vector<char> _asci_map {};

public:
  inline Asci_worms(cr<Asci_worms_context> ctx = {}): _ctx{ctx} { restart(); }

  inline void restart() override {
    assert(_ctx.font);

    _asci_map.clear();
    _asci_map.resize(_ctx.map_w * _ctx.map_h);
    std::fill(_asci_map.begin(), _asci_map.end(), ' ');

    _worms.clear();
    _worms.resize(_ctx.count);
    for (auto& w: _worms)
      w = _ctx.start_pos;
  }

  inline void draw(Image& dst) const override {
    utf32 str;
    cfor (y, _ctx.map_h) {
      cfor (x, _ctx.map_w)
        str += _asci_map.at(y * _ctx.map_w + x);
      str += U'\n';
    }
    dst.fill(Pal8::black);
    assert(_ctx.font);
    _ctx.font->draw(dst, {}, str);
  }

  void update(Delta_time _) {
    for (rauto pos: _worms) {
      rauto item = _asci_map.at(pos.x + pos.y * _ctx.map_w);
      item = ' ' + (item + 1) % (128 - ' ');
      pos.x += (rndu_fast() % 3) - 1;
      pos.y += (rndu_fast() % 3) - 1;
      pos.x = std::clamp<int>(pos.x, 0, _ctx.map_w-1);
      pos.y = std::clamp<int>(pos.y, 0, _ctx.map_h-1);
    }
  }
};

class Asci_worms_2: public Asci_worms {
  inline static Asci_worms_context _spec_ctx() {
    Asci_worms_context ctx;
    ctx.font = graphic::system_mono.get();
    ctx.map_w = 512/12 + 1;
    ctx.start_pos.x = ctx.map_w / 2;
    ctx.count = 2;
    return ctx;
  }
public:
  inline Asci_worms_2(): Asci_worms{_spec_ctx()} {}
};

class Asci_worms_3: public Asci_worms {
  inline static Asci_worms_context _spec_ctx() {
    Asci_worms_context ctx;
    ctx.font = graphic::asci_3x5.get();
    ctx.map_w = 512/4 + 1;
    ctx.map_h = 384/6 + 2;
    ctx.start_pos.x = ctx.map_w / 2;
    ctx.start_pos.y = ctx.map_h / 2;
    ctx.count = 4;
    return ctx;
  }
public:
  inline Asci_worms_3(): Asci_worms{_spec_ctx()} {}
};

// ----------------------- Регистрация фонов -----------------------------------
BGP_REG_MAKER(Simple_color)
BGP_REG_MAKER(Empty)
BGP_REG_MAKER(Repeated_circles_0)
BGP_REG_MAKER(Repeated_circles_1)
BGP_REG_MAKER(Asci_worms)
BGP_REG_MAKER(Asci_worms_2)
BGP_REG_MAKER(Asci_worms_3)

} // bgp ns
