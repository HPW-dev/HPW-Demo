#include "pch.hpp"
#include "port.hpp"

#include "port/glfw3/port-glfw.hpp"

#ifdef WINDOWS
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>

  #include <iostream>
  #include <clocale>
  #include <cstdlib>
  #include <string>
#else // Linux
  #include <iostream>
#endif


namespace Port {

#ifdef WINDOWS
// выводит окно с ошибкой
static inline void draw_error_window(cr<Str> msg) {
  std::setlocale(LC_ALL, "en_US.utf8");
  std::wstring msg_wstr;
  msg_wstr.resize(msg.size());
  std::mbstowcs(msg_wstr.data(), msg.data(), msg.size());
  MessageBoxW(NULL, msg_wstr.c_str(), L"Error", MB_ICONERROR | MB_OK);
}
#endif

void process_error(cr<Str> error) {
  std::cerr << error << std::endl;

  bool print_source_bak = hpw::logger.config.print_source;
  hpw::logger.config.print_source = false;
  log_error << error;
  hpw::logger.config.print_source = print_source_bak;

#ifdef WINDOWS
  draw_error_window(error);
#endif
}

void init() { glfw3_init(); }
bool is_ran() { return glfw3_is_ran(); }
void update() { glfw3_update(); }
void render_canvas() { glfw3_render_canvas(); }
void sync() { glfw3_sync(); }
void finalize() { glfw3_finalize(); }

} // Port ns