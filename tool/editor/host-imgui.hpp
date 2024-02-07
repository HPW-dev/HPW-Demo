#pragma once
#include "host/host-glfw.hpp"

/// Imgui рендерер с GLFW
class Host_imgui: public Host_glfw {
  void init_color_theme();
  bool inited {false};

protected:
  virtual void imgui_exec() = 0;

public:
  explicit Host_imgui(int argc_, char *argv_[]);
  ~Host_imgui();
  void init_window() override;
  void draw() override;
}; // Host_imgui
