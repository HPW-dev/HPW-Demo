#pragma once
#include "host-ogl.hpp"
#include "util/mem-types.hpp"
#include "game/util/keybits.hpp"

struct GLFWwindow;
class Key_mapper;

/// рендерер от GLFW
class Host_glfw: public Host_ogl {
public:
  Shared<Key_mapper> key_mapper {};
  
  explicit Host_glfw(int argc, char *argv[]);
  ~Host_glfw();
  void reshape(int w, int h) override;
  void set_window_pos(int x, int y) override;
  void run() override;

protected:
  int wnd_x {}, wnd_y {}; /// позиция окна на рабочем столе
  GLFWwindow* window {};
  double second_timer {}; /// для высчитывания фпс
  uint fps {};
  uint ups {};
  uint upf {};
  uint ips {};
  double frame_time {};
  double update_time {};
  double start_update_time {}; /// нужен для интерполяции движения
  bool frame_drawn {false}; /// для плавного апдейта игры

  void game_set_dt(double gameloop_time);
  void game_set_fps_info(double gameloop_time);
  void game_frame(double dt);
  void game_update(double dt);
  bool is_ran() const;
  void calc_lerp_alpha();
  void calc_upf();
  void set_update_time(double dt); /// контроль плавности апдейта

  void _set_mouse_cursour_mode(bool enable) override;
  void _set_fullscreen(bool enable) override;
  void _set_double_buffering(bool enable) override;
  virtual void init_window();
  void init_commands();
  virtual void update(double dt);
  virtual void init();
  double get_time() const override;
  /// определить какой кадр надо скипать
  void check_frame_skip();
  void frame_wait();
}; // Host_glfw
