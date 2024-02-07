#pragma once

class Image;

/// база для окошек imgui в редакторе
class Window {
public:
  virtual ~Window() = default;
  virtual void draw(Image& dst) const = 0;
  virtual void update(double dt) = 0;
  virtual void imgui_exec() = 0;
}; // Window
