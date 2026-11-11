#include "pch.hpp"
#include "port-glfw.hpp"

#include <csignal>

#ifdef WINDOWS
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>

  #define GLFW_DLL
#else
  #include <unistd.h>
#endif

#include <glad/gl.h>
// GLAD ниже GLFW не ставить. GL/glcorearb.h не подключать
#include <GLFW/glfw3.h>

#include "game/util/version.hpp"
#include "game/core/common.hpp"
#include "game/input/keys.hpp"

#ifndef DISABLE_CONFIG
  #include "game/util/config.hpp"
#endif


struct Glfw3_context {
  int argc {};
  char** argv {};

  GLFWwindow* window {};
  Veci wnd_off {440, 240}; // Стартовое смещение окна
  Str window_title {"H.P.W"};
  bool start_fullscreen {false}; // через аргументы запуска можно указать запуститься в фулскрине
#ifdef WINDOWS
  void* app_mutex {};
#endif

  bool is_ran {true};
  bool rebind_key_mode {false};
  hpw::keycode key_for_rebind {hpw::keycode::error}; // появится при hpw::rebind_key
  bool once_gamma_conf {true}; // защита от циклического выставления гаммы
  std::optional<uint32_t> custom_seed {};
};

static Glfw3_context g_ctx {}; // Все нужные данные для GLFW окна


static void init_app_mutex() {
#ifdef WINDOWS
  g_ctx.app_mutex = ::CreateMutex(NULL, TRUE, "HPW: Double window mutex");
  if (ERROR_ALREADY_EXISTS == ::GetLastError()) {
    hpw::multiple_apps = true;
    ::CloseHandle(g_ctx.app_mutex);
    log_warning << "Запущено больше двух экземпляров игры";
  }
#else
  #pragma message("need impl for other_game_started() on Linux")
#endif
}

static void free_app_mutex() {
#ifdef WINDOWS
  ::ReleaseMutex(g_ctx.app_mutex);
  ::CloseHandle(g_ctx.app_mutex);
#else
  #pragma message("need impl for free_app_mutex() on Linux")
#endif
}

// Обработчик для Ctrl+C
static void signal_handler(int signal) {
  if (signal == SIGINT) {
    log_info << "Закрытие через SIGINT...";
    g_ctx.is_ran = false;
  }
}

static void parse_args(int argc, char** argv) {
  Pparser ret( Pparser::v_param_t {
    {{"-s", "--seed"}, "set random seed", [](cr<Str> val){
      g_ctx.custom_seed = s2n<uint32_t>(val);
    }},
    {{"-ox", "--offset_x"}, "set window startup offset by x", [](cr<Str> val){
      g_ctx.wnd_off.x = s2n<uint32_t>(val);
    }},
    {{"-oy", "--offset_y"}, "set window startup offset by y", [](cr<Str> val){
      g_ctx.wnd_off.y = s2n<uint32_t>(val);
    }},
    {{"-w", "--windowed"}, "enable windowed mode", [](cr<Str> val){
      g_ctx.start_fullscreen = false;
    }},
    {{"-f", "--fullscreen"}, "enable fullscreen mode", [](cr<Str> val){
      g_ctx.start_fullscreen = true;
    }},
    {{"-d", "--debug"}, "enable debug log stream", [](cr<Str> _){
      hpw::logger.config.use_stream_debug = true;
    }},
    {{"-h", "--help", "--info"}, "print this help and exit", [&](cr<Str> val){
      ret.print_info();
      std::exit(EXIT_SUCCESS);
    }},
    {{"-t", "--title"}, "set window title (if not defined, set HPW)", [](cr<Str> val) {
      if (val.empty())
        g_ctx.window_title = val;
    }},
    {{"-v", "--version"}, "print game version and exit", [](cr<Str> val){
      cauto ver = get_game_version();
      cauto creation_date = get_game_creation_date();
      cauto creation_time = get_game_creation_time();
      log_info << std::format("game version: {} ({} {})", ver, creation_date, creation_time);
      std::exit(EXIT_SUCCESS);
    }},
  } );

  ret.skip_empty = true;
  ret(argc, argv);
}

//---- Основные функции GLFW3 порта --------------------------------------------
void glfw3_init(int argc, char** argv) {
  log_info << "GLFW3 initialization...";

  std::signal(SIGINT, signal_handler); // для закрытия через Ctrl+C 
  
  // аргументы запуска:
  hpw::argc = g_ctx.argc = argc;
  hpw::argv = g_ctx.argv = argv;
  parse_args(argc, argv);

  init_app_mutex(); // предупреждаем о двух окнах игры

  log_debug << "GLFW3 end of initialization";
}

bool glfw3_is_ran() {
  return g_ctx.is_ran && (glfwWindowShouldClose(g_ctx.window) == GL_FALSE);
}

void glfw3_update() {
  // TODO
}

void glfw3_render_canvas() {
  // TODO
}

void glfw3_sync() {
  // TODO
}

void glfw3_finalize() {
  log_debug << "GLFW3 finalization...";
  
#ifndef DISABLE_CONFIG
  save_config();
#endif
  
  free_app_mutex();
  
  log_debug << "GLFW3 end of finalization";
  log_info << "Корректное завершение";
}
