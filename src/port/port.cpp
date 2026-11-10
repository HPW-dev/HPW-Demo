#include "pch.hpp"
#include "port.hpp"

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
#endif

// выводит окно с ошибкой
static inline void draw_error_window(cr<Str> msg) {
#ifdef WINDOWS
  std::setlocale(LC_ALL, "en_US.utf8");
  std::wstring msg_wstr;
  msg_wstr.resize(msg.size());
  std::mbstowcs(msg_wstr.data(), msg.data(), msg.size());
  MessageBoxW(NULL, msg_wstr.c_str(), L"Error", MB_ICONERROR | MB_OK);
#endif
}

namespace Port {

void process_error(cr<Str> error) {
  std::cerr << error << std::endl;
  log_error << error;
  draw_error_window(error);
}

} // Port ns