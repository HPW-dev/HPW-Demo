#include "dbg-plots.hpp"

struct Dbg_plots::Impl {
  inline void draw(Image& dst) const {}
  inline void update(Delta_time dt) {}
};

Dbg_plots::Dbg_plots(): impl{new_unique<Impl>()} {}
Dbg_plots::~Dbg_plots() {}
void Dbg_plots::draw(Image& dst) const { impl->draw(dst); }
void Dbg_plots::update(Delta_time dt) { impl->update(dt); }
