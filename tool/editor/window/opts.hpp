#pragma once
#include "window.hpp"

// глобальные настройки редактора
class Opts_wnd: public Window {
public:
  Opts_wnd() = default;
  ~Opts_wnd() = default;
  void imgui_exec() override;
  inline void draw(Image& dst) const {}
  inline void update(double dt) {}
};
