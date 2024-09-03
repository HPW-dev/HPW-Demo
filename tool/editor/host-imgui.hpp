#pragma once
#include "host/glfw3/host-glfw.hpp"

// Imgui рендерер с GLFW
class Host_imgui: public Host_glfw {
  void init_color_theme();
  bool inited {false};

protected:
  virtual void imgui_exec() const = 0;
  void draw_game_frame() const override;

public:
  explicit Host_imgui(int argc_, char *argv_[]);
  ~Host_imgui();
  void init_window() override;
}; // Host_imgui
