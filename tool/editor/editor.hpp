#pragma once
#include "host-imgui.hpp"

// мейник для редактора
class Editor final: public Host_imgui {
  nocopy(Editor);

protected:
  void update(double dt) override;
  void draw() override;
  void imgui_exec() override;
  void init() override;
  void init_graphic();

public:
  Editor(int argc, char *argv[]);
  ~Editor() = default;
}; // Editor
