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

#include "engine/graphic/image/color-rgb.hpp"
#include "engine/graphic/image/palette.hpp"
#include "game/util/screenshot.hpp"
#include "game/util/version.hpp"
#include "game/util/logo.hpp"
#include "game/core/core-window.hpp"
#include "game/core/graphic.hpp"
#include "game/core/common.hpp"
#include "game/core/canvas.hpp"
#include "game/input/keys.hpp"
#include "port/port.hpp"

#ifndef DISABLE_CONFIG
  #include "game/util/game-archive.hpp"
  #include "game/util/config.hpp"
#endif


//---- Все нужные данные для GLFW окна -----------------------------------------
struct Glfw3_context {
  int argc {};
  char** argv {};

// GLFW окно:
  GLFWwindow* window {};
  Veci wnd_off {440, 240}; // Стартовое смещение окна
  Veci wnd_sz {graphic::width, graphic::height}; // реальная ширина и высота окна (меняется при ресайзе)
  Recti wnd_game_area {}; // видимая часть внутреннего окна игры
  Str window_title {"H.P.W"};
  bool start_fullscreen {false}; // через аргументы запуска можно указать запуститься в фулскрине
#ifdef WINDOWS
  void* app_mutex {};
#endif

// OpenGL:
  Vector<GLfloat> pal_rgb {}; // Палитра игры подготовленная к отправке на видяху
  uint screen_tex = 0; // OpenGL текстура экрана
  uint vao = 0;
  GLuint pbos[2] = {0, 0};
  bool use_double_pbo {true};
  int pbo_index = 0;
  size_t pbo_sz = 0;
  uint shader_prog = 0, vert_shader = 0, frag_shader = 0;
  uint pal_tex = 0;
  cp<void> pixels {}; // данные для копирования в текстуру

// Общее для игры:
  bool is_ran {true};
  bool rebind_key_mode {false};
  hpw::keycode key_for_rebind {hpw::keycode::error}; // появится при hpw::rebind_key
  bool once_gamma_conf {true}; // защита от циклического выставления гаммы
  std::optional<uint32_t> custom_seed {};
};
static Glfw3_context g_ctx {};

//---- Внутренние функции порта ------------------------------------------------

static bool is_bad_wnd_sz() {
  return
    g_ctx.wnd_sz.x <= 0 ||
    g_ctx.wnd_sz.y <= 0 ||
    g_ctx.wnd_game_area.size.x <= 0 ||
    g_ctx.wnd_game_area.size.y <= 0; 
}

// растягивает внутреннее окно игры с учётом соотношений и режимов
static void apply_resize_mode() {
  constexpr int base_w = graphic::width;
  constexpr int base_h = graphic::height;
  constexpr real aspect = scast<real>(base_w) / scast<real>(base_h);

  // если окно винды свернуть, то оно станет нулевого размера. Защита от вылета:
  if (g_ctx.wnd_sz.x <= 0 || g_ctx.wnd_sz.y <= 0) {
    g_ctx.wnd_game_area.pos = {0, 0};
    g_ctx.wnd_game_area.size = {0, 0};
    return;
  }

  Veci new_sz {};

  switch (graphic::resize_mode) {
    default:
    case Resize_mode::one_to_one: {
      // максимальный множитель, при котором игра целиком влезает в окно
      const int scale_x = g_ctx.wnd_sz.x / base_w;
      const int scale_y = g_ctx.wnd_sz.y / base_h;
      int scale = std::min(scale_x, scale_y);

      // выбираем наибольший кратный 2 множитель или 1
      if (scale >= 2)
        scale = (scale / 2) * 2; // нечётные -> чётные
      else
        scale = 1;

      new_sz.x = base_w * scale;
      new_sz.y = base_h * scale;
      break;
    }

    case Resize_mode::by_width: {
      new_sz.x = g_ctx.wnd_sz.x;
      new_sz.y = scast<int>(new_sz.x / aspect);
      break;
    }

    case Resize_mode::by_height: {
      new_sz.y = g_ctx.wnd_sz.y;
      new_sz.x = scast<int>(new_sz.y * aspect);
      break;
    }

    case Resize_mode::full: {
      new_sz = g_ctx.wnd_sz;
      break;
    }
  } // switch resize mode

  // Центрирование внутреннего окна игры в физическом внешнем:
  g_ctx.wnd_game_area.size = new_sz;
  g_ctx.wnd_game_area.pos.x = (g_ctx.wnd_sz.x - new_sz.x) / 2;
  g_ctx.wnd_game_area.pos.y = (g_ctx.wnd_sz.y - new_sz.y) / 2;
}

static void soft_exit() {
  log_info << "software exit...";
  g_ctx.is_ran = false;
}

static void reshape(int w, int h) {
  g_ctx.wnd_sz.x = w;
  g_ctx.wnd_sz.y = h;
  apply_resize_mode();

  // окно нулевого размера не надо отображать
  graphic::enable_render = !is_bad_wnd_sz();
  if (graphic::enable_render) {
    // настройка 2D координат и проекций OpenGL:
    glViewport(
      g_ctx.wnd_game_area.pos.x,
      g_ctx.wnd_game_area.pos.y,
      g_ctx.wnd_game_area.pos.x + g_ctx.wnd_game_area.size.x,
      g_ctx.wnd_game_area.pos.y + g_ctx.wnd_game_area.size.y);
  }

  log_debug << "new window size: " << g_ctx.wnd_sz.x << ", " << g_ctx.wnd_sz.y
    << " (area: " << to_str(g_ctx.wnd_game_area.pos) << ", " << to_str(g_ctx.wnd_game_area.size) << ")";
}

static void set_resize_mode(Resize_mode mode) {
  log_debug << "new resize mode: " << scast<int>(mode);
  graphic::resize_mode = mode;
  reshape(graphic::width, graphic::height);
  
  /* если фулскрин был включён, то сначала выключить его,
  иначе на линуксе экран погаснет */
  if (graphic::fullscreen) {
    Port::toggle_fullscreen(false);
    Port::toggle_fullscreen(true);
  }
}

// привязка функций порта
static void init_port_cbs() {
  log_debug << "init port callbacks...";

  Port::soft_exit = &soft_exit;
  Port::make_screenshot = []{ save_screenshot(graphic::canvas); };
  Port::set_resize_mode = &set_resize_mode;
  /* TODO
  Port::rebind_key = ;
  Port::rebind_key_by_scancode = ;
  Port::reset_keymap = ;
  Port::toggle_vsync = ;
  Port::set_palette = ;
  Port::set_gamma = ;
  Port::toggle_fullscreen = ;
  Port::toggle_double_buffering = ;
  Port::toggle_mouse_visible = ;
  Port::set_target_ups = ;
  Port::get_time = ;
  */
}

// рандомный логотип в консоли
static void print_logo() {
  cauto print_source = hpw::logger.config.print_source;
  cauto print_stream_name = hpw::logger.config.print_stream_name;
  hpw::logger.config.print_source = false;
  hpw::logger.config.print_stream_name = false;

  log_info << get_random_logo();

  hpw::logger.config.print_source = print_source;
  hpw::logger.config.print_stream_name = print_stream_name;
}

// настроить сид рандома
static void init_random() {
  log_debug << "init random seed...";

  u32_t seed;
  if (g_ctx.custom_seed.has_value()) {
    seed = g_ctx.custom_seed.value();
  } else {
    seed = std::time(nullptr);
  }

  // TODO учесть большой сид
  log_debug << std::format("seed: {}\n", seed);
  set_rnd_seed({.old_seed=seed});
}

// для предупреждения о двух окнах игры
static void init_app_mutex() {
#ifdef WINDOWS
  g_ctx.app_mutex = ::CreateMutex(NULL, TRUE, "HPW: Double window mutex");
  if (::GetLastError() == ERROR_ALREADY_EXISTS) {
    hpw::multiple_apps = true;
    ::CloseHandle(g_ctx.app_mutex);
    g_ctx.app_mutex = nullptr;
    log_warning << "Запущено больше двух экземпляров игры";
  }
#else
  #pragma message("need impl for other_game_started() on Linux")
#endif
}

static void free_app_mutex() {
#ifdef WINDOWS
  if (g_ctx.app_mutex) {
    ::ReleaseMutex(g_ctx.app_mutex);
    ::CloseHandle(g_ctx.app_mutex);
  }
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

static void process_args(int argc, char** argv) {
  hpw::argc = g_ctx.argc = argc;
  hpw::argv = g_ctx.argv = argv;

  Pparser ret( Pparser::v_param_t {
    {{"-s", "--seed"}, "set random seed", [](cr<Str> val){
      g_ctx.custom_seed = s2n<u32_t>(val);
    }},
    {{"-ox", "--offset_x"}, "set window startup offset by x", [](cr<Str> val){
      g_ctx.wnd_off.x = s2n<int>(val);
    }},
    {{"-oy", "--offset_y"}, "set window startup offset by y", [](cr<Str> val){
      g_ctx.wnd_off.y = s2n<int>(val);
    }},
    {{"-w", "--windowed"}, "enable windowed mode", [](cr<Str> val){
      g_ctx.start_fullscreen = false;
    }},
    {{"-f", "--fullscreen"}, "enable fullscreen mode", [](cr<Str> val){
      g_ctx.start_fullscreen = true;
    }},
    {{"-d", "--debug"}, "enable debug log stream", [](cr<Str> _){
      hpw::logger.config.use_stream_debug = true;
      hpw::forced_debug_log = true;
    }},
    {{"-h", "--help", "--info"}, "print this help and exit", [&](cr<Str> val){
      ret.print_info();
      std::exit(EXIT_SUCCESS);
    }},
    {{"-t", "--title"}, "set window title (if not defined, set HPW)", [](cr<Str> val) {
      if (!val.empty())
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

static void init_rgb_pal() {
  log_debug << "init rgb (float) palette for OGL...";
  cauto default_pal = get_default_palette();
  g_ctx.pal_rgb.resize(default_pal.size() * 3);

  for (size_t i = 0; crauto c: default_pal) {
    g_ctx.pal_rgb.at(i * 3 + 0) = c.r / 255.0;
    g_ctx.pal_rgb.at(i * 3 + 1) = c.g / 255.0;
    g_ctx.pal_rgb.at(i * 3 + 2) = c.b / 255.0;
    ++i;
  }
}

//---- Основные функции GLFW3 порта --------------------------------------------
void glfw3_init(int argc, char** argv) {
  log_info << "GLFW3 initialization...";

  std::signal(SIGINT, signal_handler); // для закрытия через Ctrl+C 

#ifdef WINDOWS
  // переключить консоль винды в UTF8
  SetConsoleOutputCP(CP_UTF8);
#endif

  process_args(argc, argv); // парс аргументов <--------------------------------
  hpw::cur_dir = launch_dir_from_argv0(argv); // узнать в какой папке игра запущена
  log_debug << "launch directory: " << hpw::cur_dir;

  init_app_mutex();
  init_random();
  print_logo();
#ifndef DISABLE_CONFIG
  load_config();
#endif
  init_port_cbs();

  g_ctx.pixels = scast<cp<void>>(graphic::canvas.data());

#ifndef DISABLE_CONFIG
  init_archive(); // из архива понадобятся шейдеры
#endif
#ifndef ECOMEM
  load_color_tables();
#endif

  log_debug << "GLFW3 end of initialization";
}

bool glfw3_is_ran() {
  return g_ctx.is_ran && g_ctx.window && (glfwWindowShouldClose(g_ctx.window) == GL_FALSE);
}

void glfw3_update() {
  // TODO update states
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
  
  log_info << "Корректное завершение";
}
