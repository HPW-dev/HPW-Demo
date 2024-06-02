#pragma once
#include "host-ogl.hpp"
#include "util/mem-types.hpp"
#include "game/util/keybits.hpp"

struct GLFWwindow;
class Key_mapper;

// рендерер от GLFW
class Host_glfw: public Host_ogl {
public:
  Shared<Key_mapper> key_mapper {};
  
  explicit Host_glfw(int argc, char *argv[]);
  ~Host_glfw();
  void reshape(int w, int h) override;
  void set_window_pos(int x, int y) override;
  void run() override;

protected:
  int wnd_x {}, wnd_y {}; // позиция окна на рабочем столе
  GLFWwindow* window {};
  Delta_time second_timer {}; // для высчитывания фпс
  uint fps {};
  uint ups {};
  uint upf {};
  uint ips {};
  Delta_time frame_time {};
  Delta_time update_time {};
  Delta_time start_update_time {}; // нужен для интерполяции движения
  bool frame_drawn {false}; // для плавного апдейта игры

  void game_set_dt(const Delta_time gameloop_time);
  void game_set_fps_info(const Delta_time gameloop_time);
  void game_frame(const Delta_time dt);
  void game_update(const Delta_time dt);
  bool is_ran() const;
  void calc_lerp_alpha();
  void calc_upf();
  void set_update_time(const Delta_time dt); // контроль плавности апдейта
  void apply_render_delay();
  void apply_update_delay();

  void _set_mouse_cursour_mode(bool enable) override;
  void _set_fullscreen(bool enable) override;
  void _set_double_buffering(bool enable) override;
  void set_gamma(const double gamma) override;
  virtual void init_window();
  void init_commands();
  virtual void update(const Delta_time dt);
  virtual void init();
  Delta_time get_time() const override;
  // определить какой кадр надо скипать
  void check_frame_skip();
  void frame_wait();
}; // Host_glfw
