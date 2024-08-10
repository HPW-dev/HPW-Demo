#include <deque>
#include <functional>
#include "dbg-plots.hpp"
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

  inline explicit Plotter(Getter&& getter, uint w, uint h, uint len, const Vec offset) {
    // TODO
  }

  inline void update() {
    // TODO
  }

  inline void draw(Image& dst) const {
    // TODO
  }

private:
  Getter m_getter {};
  uint m_w {};
  uint m_h {};
  Vec m_offset {};
  std::deque<Value> m_values {};
}; // Plotter

struct Dbg_plots::Impl {
  Vector<Plotter> m_plotters {};

  inline Impl() {

  }

  inline void draw(Image& dst) const {
    for (cnauto plotter: m_plotters)
      plotter.draw(dst);
  }

  inline void update(Delta_time dt) {
    for (nauto plotter: m_plotters)
      plotter.update(dt);
  }
}; // Impl

Dbg_plots::Dbg_plots(): impl{new_unique<Impl>()} {}
Dbg_plots::~Dbg_plots() {}
void Dbg_plots::draw(Image& dst) const { impl->draw(dst); }
void Dbg_plots::update(Delta_time dt) { impl->update(dt); }
