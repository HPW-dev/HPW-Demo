#pragma once
#include "util/math/num-types.hpp"

class Image;

// база для окошек imgui в редакторе
class Window {
public:
  virtual ~Window() = default;
  inline virtual void draw(Image& dst) const {};
  inline virtual void update(const Delta_time dt) {};
  virtual void imgui_exec() = 0;
}; // Window
