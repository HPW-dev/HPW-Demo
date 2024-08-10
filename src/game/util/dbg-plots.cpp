#include <algorithm>
#include <cassert>
#include <deque>
#include <cmath>
#include <utility>
#include <functional>
#include "dbg-plots.hpp"
#include "game/core/fonts.hpp"
#include "game/core/core.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/util/sync.hpp"
#include "game/core/graphic.hpp"

// рисует графики
class Plotter {
public:
  using Value = real;
  using Getter = std::function<Value ()>;

  /** окошко с графиком
  @param title название для графика
  @param getter получает значения для графика при вызове update
  @param w ширина окна с графиком
  @param h высота окна с графиком
  @param len разрешение графика. Если = 0, разрешение станет = w
  @param offset с каким смещением будет отрисовано окно графика */
  inline explicit Plotter(CN<utf32> title, Getter&& getter, uint w, uint h, uint len, const Vec offset)
  : m_title {title}
  , m_getter {std::move(getter)}
  , m_w {w}
  , m_h {h}
  , m_offset {offset}
  , m_values(len)
  {
    assert(len < 4'000'000u);
    if (len == 0) {
      len = w;
      m_values.resize(len);
    }
    assert(m_w > 0 && m_w < 600);
    assert(m_h > 0 && m_h < 400);
    assert(m_getter);
    for (nauto val: m_values)
      val = 0;
    update_maximum();
  }

  inline void update() {
    m_values.pop_front();
    m_values.emplace_back(m_getter());
    update_maximum();
  }

  inline void draw(Image& dst) const {
    assert(dst);
    // окно графика
    Rect window(m_offset, Vec(m_w, m_h));
    draw_rect(dst, window, Pal8::white);
    // отрисовка графика
    assert(m_values.size() > 2);
    cfor (i, m_values.size() - 1) {
      cauto val_0 = m_values[i];
      cauto val_1 = m_values[i+1];
      const uint x_0 = std::floor(m_offset.x + (scast<real>(i) / m_values.size()) * m_w);
      const uint x_1 = std::floor(m_offset.x + (scast<real>(i+1) / m_values.size()) * m_w);
      const uint y_0 = std::floor(m_offset.y + (1 - (val_0 / m_maximum)) * m_h);
      const uint y_1 = std::floor(m_offset.y + (1 - (val_1 / m_maximum)) * m_h);
      draw_line(dst, Vec(x_0, y_0), Vec(x_1, y_1), Pal8::red);
    }
    // название окна
    assert(graphic::font);
    graphic::font->draw(dst, m_offset + Vec(5, 5), m_title, &blend_diff);
    // размерности
    graphic::font->draw(dst, m_offset + Vec(m_w + 5, 0), n2s<utf32>(m_maximum), &blend_diff);
    graphic::font->draw(dst, m_offset + Vec(m_w + 5, m_h/2), n2s<utf32>(m_maximum/2), &blend_diff);
  }

private:
  utf32 m_title {};
  Getter m_getter {};
  uint m_w {};
  uint m_h {};
  Vec m_offset {};
  std::deque<Value> m_values {};
  real m_maximum {};

  inline void update_maximum() {
    auto it = std::max_element(m_values.begin(), m_values.end());
    assert(it != m_values.end());
    m_maximum = *it;
  }
}; // Plotter

struct Dbg_plots::Impl {
  Vector<Plotter> m_plotters {}; // графики, которые надо нарисовать

  inline Impl() {
    m_plotters = {
      Plotter(U"тик",            []->Plotter::Value { return hpw::tick_time; },          180, 60, 240*4, {5, 5}),
      Plotter(U"тик зум",        []->Plotter::Value { return hpw::tick_time; },          180, 60, 30,    {260, 5}),
      Plotter(U"сфот. кадр",     []->Plotter::Value { return graphic::soft_draw_time; }, 180, 66, 240*6, {5, 75}),
      Plotter(U"сфот. кадр зум", []->Plotter::Value { return graphic::soft_draw_time; }, 180, 66, 60,    {260, 75}),
      Plotter(U"хард. кадр",     []->Plotter::Value { return graphic::hard_draw_time; }, 180, 66, 240*6, {5, 150}),
      Plotter(U"хард. кадр зум", []->Plotter::Value { return graphic::hard_draw_time; }, 180, 66, 30,    {260, 150}),
      Plotter(U"FPS",            []->Plotter::Value { return graphic::cur_fps; },        180, 60, 240*6, {5, 225}),
    };
  }

  inline void draw(Image& dst) const {
    apply_brightness(dst, -97);
    
    for (cnauto plotter: m_plotters)
      plotter.draw(dst);
  }

  inline void update(Delta_time dt) {
    for (nauto plotter: m_plotters)
      plotter.update();
  }
}; // Impl

Dbg_plots::Dbg_plots(): impl{new_unique<Impl>()} {}
Dbg_plots::~Dbg_plots() {}
void Dbg_plots::draw(Image& dst) const { impl->draw(dst); }
void Dbg_plots::update(Delta_time dt) { impl->update(dt); }
