#pragma once

#ifdef HOST_GLFW3
#include "host/glfw3/host-glfw.hpp"
using Host_class = Host_glfw;
#endif
#ifdef HOST_SDL2
#error "need impl for SDL2"
using Host_class = Host_sdl2;
#endif
#ifdef HOST_ASCI
#include "host/asci/host-asci.hpp"
using Host_class = Host_asci;
#endif

class Image;

class Game_app: public Host_class {
  nocopy(Game_app);
  
  void update(const Delta_time dt) override;
  void load_fonts();
  void update_graphic_autoopt(const Delta_time dt);
  // полноэкранная рамка
  void draw_border(Image& dst) const;
  void draw_game_frame() const override;
  void check_errors();
  void replay_save_keys();
  void replay_load_keys();

public:
  explicit Game_app(int argc, char *argv[]);
  ~Game_app();
}; // Game_app

