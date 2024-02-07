#pragma once
#include "host/host-glfw.hpp"

class Image;

class Game_app: public Host_glfw {
  nocopy(Game_app);
  
  void update(double dt) override;
  void load_locale();
  void load_font();
  void update_graphic_autoopt(double dt);
  /// полноэкранная рамка
  void draw_border(Image& dst) const;

public:
  explicit Game_app(int argc, char *argv[]);
  ~Game_app() = default;
  void draw() override;
}; // Game_app

