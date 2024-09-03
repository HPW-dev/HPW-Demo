#pragma once
#include "host-imgui.hpp"

// мейник для редактора
class Editor final: public Host_imgui {
  nocopy(Editor);

protected:
  void update(const Delta_time dt) override;
  void imgui_exec() const override;
  void init() override;
  void init_graphic();
  void draw_game_frame() const override;

public:
  Editor(int argc, char *argv[]);
  ~Editor() = default;
}; // Editor
