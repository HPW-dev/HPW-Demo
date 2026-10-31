#include "pch.hpp"
#include "plotter.hpp"

#include "engine/graphic/util/util-templ.hpp"
#include "engine/graphic/image/image.hpp"
#include "game/core/fonts.hpp"


Plotter::Plotter(cr<utf32> title, Getter&& getter, uint w, uint h,
uint len, const Vec offset)
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
  for (rauto val: m_values)
    val = 0;
  update_maximum();
}

void Plotter::update() {
  m_values.pop_front();
  m_values.emplace_back(m_getter());
  update_maximum();
}

void Plotter::draw(Image& dst) const {
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

void Plotter::update_maximum() {
  auto it = std::max_element(m_values.begin(), m_values.end());
  assert(it != m_values.end());
  m_maximum = *it;
}
