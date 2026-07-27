#pragma once
#include "window.hpp"

// верхнее меню
class Menu_wnd: public Window {
  void back_from_wnd();

public:
  Menu_wnd() = default;
  ~Menu_wnd() = default;
  inline void draw(Image& dst) const override {}
  void update(const Delta_time dt) override;
  void imgui_exec() override;
};
