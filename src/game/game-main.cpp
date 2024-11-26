#include <iostream>
#include "game-app.hpp"
#include "util/error.hpp"
#include "util/platform.hpp"
#include "util/log.hpp"

#ifdef WINDOWS
#include <clocale>
#include <cstdlib>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
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

static inline void process_error(cr<Str> error) {
  std::cerr << error << std::endl;
  hpw_log(error, Log_stream::warning);
  draw_error_window(error);
}

int main(int argc, char *argv[]) {
  try {
    Game_app app(argc, argv);
    app.run();
    return EXIT_SUCCESS;
  } catch(cr<hpw::Error> err) {
    const Str err_str = err.what();
    process_error(err_str);
  } catch(cr<std::exception> err) {
    const Str err_str = Str{"STD Error: "} + err.what();
    process_error(err_str);
  } catch(...) {
    cauto err_str = "Unknown error";
    process_error(err_str);
  }

  return EXIT_FAILURE;
}
