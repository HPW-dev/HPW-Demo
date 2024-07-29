#pragma once
#include "host/glfw3/host-glfw.hpp"

class Image;

class Game_app: public Host_glfw {
  nocopy(Game_app);
  
  void update(const Delta_time dt) override;
  void load_locale();
  void load_font();
  void update_graphic_autoopt(const Delta_time dt);
  // полноэкранная рамка
  void draw_border(Image& dst) const;
  void draw_game_frame() override;
  void check_errors();

public:
  explicit Game_app(int argc, char *argv[]);
  ~Game_app();
}; // Game_app

