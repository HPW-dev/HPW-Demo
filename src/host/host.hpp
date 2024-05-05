#pragma once
#include <atomic>
#include "util/macro.hpp"
#include "resize.hpp"
#include "command.hpp"

// базовый класс для игрового хоста
class Host {
  void callbacks_init(); // задать колбэки для общих команд

protected:
  int argc {};
  char** argv {};
  std::atomic_bool is_run {true};
  
  virtual void exit();
  virtual void _set_fullscreen(bool enable) = 0;
  virtual void save_screenshot() const = 0;
  virtual void _set_double_buffering(bool enable) = 0;
  virtual void _set_resize_mode(Resize_mode mode) = 0;
  virtual void _set_mouse_cursour_mode(bool mode) = 0;
  virtual void set_gamma(const double gamma) = 0;
  // получить текущее время в секундах
  virtual double get_time() const = 0;
  inline virtual void draw_game_frame() {}

public:
  explicit Host(int _argc, char** _argv);
  ~Host();
  inline virtual void run() {}
}; // Host
