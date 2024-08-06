#pragma once
#include <atomic>
#include "util/macro.hpp"
#include "util/math/num-types.hpp"
#include "host-resize.hpp"
#include "command.hpp"

// базовый класс для игрового хоста
class Host {
  void callbacks_init(); // задать колбэки для общих команд

protected:
  int m_argc {};
  char** m_argv {};
  std::atomic_bool m_is_ran {true};

  #ifdef WINDOWS
  void* m_app_mutex {};
  #endif
  
  virtual void exit();
  virtual void _set_fullscreen(bool enable) = 0;
  virtual void _set_double_buffering(bool enable) = 0;
  virtual void _set_resize_mode(Resize_mode mode) = 0;
  virtual void _set_mouse_cursour_mode(bool mode) = 0;
  virtual void set_gamma(const double gamma) = 0;
  // получить текущее время в секундах
  virtual Delta_time get_time() const = 0;
  inline virtual void draw_game_frame() const {}
  void init_app_mutex();
  void free_app_mutex();

public:
  explicit Host(int argc, char** argv);
  ~Host();
  inline virtual void run() {}
  inline virtual void update(const Delta_time dt) {}
}; // Host
