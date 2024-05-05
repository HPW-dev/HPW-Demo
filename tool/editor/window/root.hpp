#pragma once
#include "window.hpp"
#include "util/mem-types.hpp"
#include "util/vector-types.hpp"

// корневое окно редактора
class Root_wnd: public Window {
  Vector<Shared<Window>> wnds {}; // all windows
  real other_bg_pos {};

  void init_root_wnd();
  void draw_zoom_x2(Image& dst) const;
  void draw_cross_bg(Image& dst) const;
  void draw_cross_fg(Image& dst) const;
  void draw_bg(Image& dst) const;
  void draw_pause(Image& dst) const;
  void draw_hitboxes(Image& dst) const;

public:
  Root_wnd();
  ~Root_wnd() = default;
  void draw(Image& dst) const override;
  void update(double dt) override;
  void imgui_exec() override;
};
