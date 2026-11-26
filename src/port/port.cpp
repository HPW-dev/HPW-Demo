#include "pch.hpp"
#include "port.hpp"

#include "port/glfw3/port-glfw.hpp"
#include "game/core/common.hpp"

#ifdef WINDOWS
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>
  #include <timeapi.h>

  #include <iostream>
  #include <clocale>
  #include <cstdlib>
  #include <string>
#else // Linux
  #include <iostream>
  #include <unistd.h>
#endif


namespace Port {

#ifdef WINDOWS
static void* g_app_mutex {};
#endif

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

static void init_platform() {
  log_info << "platform initialization...";
  std::setlocale(LC_ALL, "C"); // чтобы нормально парсить дробные числа

#ifdef WINDOWS
  // переключить консоль винды в UTF8
  ::SetConsoleOutputCP(CP_UTF8);
  ::SetConsoleCP(CP_UTF8);

  auto hOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut != INVALID_HANDLE_VALUE) {
    DWORD mode = 0;
    if (::GetConsoleMode(hOut, &mode))
      ::SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
  }

  ::timeBeginPeriod(1); // Повысит точность таймера Windows до 1 мс

  // не предлагать краш-дампы и зависшее окно "Не отвечает"
  ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif
}

static void finalize_platform() {
#ifdef WINDOWS
  ::timeEndPeriod(1);
#endif
}

// для предупреждения о двух окнах игры
static void init_app_mutex() {
#ifdef WINDOWS
  g_app_mutex = ::CreateMutex(NULL, TRUE, "HPW: Double window mutex");
  if (::GetLastError() == ERROR_ALREADY_EXISTS) {
    hpw::multiple_apps = true;
    ::CloseHandle(g_app_mutex);
    g_app_mutex = nullptr;
    log_warning << "Запущено больше двух экземпляров игры";
  }
#else
  #pragma message("need impl for other_game_started() on Linux")
#endif
}

static void free_app_mutex() {
#ifdef WINDOWS
  if (g_app_mutex) {
    ::ReleaseMutex(g_app_mutex);
    ::CloseHandle(g_app_mutex);
  }
#else
  #pragma message("need impl for free_app_mutex() on Linux")
#endif
}


void init(int argc, char** argv) {
  init_platform();
  init_app_mutex();
  glfw3_init(argc, argv);
}

bool is_ran() { return glfw3_is_ran(); }
void update() { glfw3_update(); }
void render_canvas() { glfw3_render_canvas(); }
void sync() { glfw3_sync(); }

void finalize() {
  finalize_platform();
  glfw3_finalize();
  free_app_mutex();
  log_info << "Корректное завершение";
}

} // Port ns