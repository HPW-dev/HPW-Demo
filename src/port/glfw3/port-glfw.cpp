#include "pch.hpp"
#include "glfw-context.hpp"
#include "glfw-render.hpp"
#include "port-glfw.hpp"

#include <csignal>
#include "glfw-include.hpp"

#include "game/util/screenshot.hpp"
#include "game/util/version.hpp"
#include "game/util/sync.hpp"
#include "game/core/graphic.hpp"
#include "game/core/common.hpp"
#include "game/core/user.hpp"
#include "game/core/core.hpp"
#include "port/port.hpp"

#ifndef DISABLE_CONFIG
  #include "game/util/game-archive.hpp"
  #include "game/util/config.hpp"
#endif


//---- Внутренние функции порта ------------------------------------------------

static void soft_exit() {
  log_info << "software exit...";
  g_ctx.is_ran = false;
}

static void set_target_ups(int new_ups) {
  assert(new_ups > 0);
  hpw::target_ups = new_ups;
  hpw::target_tick_time = 1.0 / hpw::target_ups;
}

// привязка функций порта
static void init_port_cbs() {
  log_debug << "init port callbacks...";

  // misc
  Port::make_screenshot = []{ save_screenshot(graphic::canvas); };
  Port::set_target_ups = &set_target_ups;
  Port::soft_exit = &soft_exit;
  Port::get_time = &glfwGetTime;
  
  // graphic
  Port::toggle_double_buffering = &toggle_double_buffering;
  Port::toggle_mouse_visible = &toggle_mouse_visible;
  Port::toggle_fullscreen = &toggle_fullscreen;
  Port::set_resize_mode = &set_resize_mode;
  Port::toggle_vsync = &toggle_vsync;
  Port::set_palette = &set_palette;
  Port::set_gamma = &set_gamma;

  // input
  Port::rebind_key_by_scancode = &rebind_key_by_scancode;
  Port::rebind_key = &rebind_key;
  Port::reset_keymap = &reset_keymap;
}

// настроить сид рандома
static void init_random() {
  log_debug << "init random seed...";

  u32_t seed;
  if (g_ctx.custom_seed.has_value()) {
    seed = g_ctx.custom_seed.value();
  } else {
    cauto now = std::chrono::steady_clock::now().time_since_epoch().count();
    // splitmix64:
    auto x = scast<u64_t>(now);
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    seed = scast<u32_t>(x ^ (x >> 31));
  }

  // TODO учесть большой сид
  log_debug << std::format("seed: {}\n", seed);
  set_rnd_seed({.old_seed=seed});
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

// колбэк для вывода ошибок GLFW
static void error_cb(int error, Cstr description) {
  // поставить дефолтную гамму при ошибке
  if (g_ctx.window && g_ctx.once_gamma_conf) {
    set_gamma(1);
    g_ctx.once_gamma_conf = false;
  }

  Str addition_desc;
  if (error == 65540)
    addition_desc += ". Maybe need delete config/config.json";

  error("GLFW error: " << error << ": " << description << addition_desc);
}

static void init_port(int argc, char** argv) {
  log_info << "Common port initialization...";

  std::signal(SIGINT, signal_handler); // для закрытия через Ctrl+C 

  process_args(argc, argv); // парс аргументов <--------------------------------
  hpw::cur_dir = get_launch_dir(); // узнать в какой папке игра запущена
  log_debug << "launch directory: " << hpw::cur_dir;

  init_random();
  print_logo();

#ifndef DISABLE_CONFIG
  load_config();
#endif

  if (g_ctx.start_fullscreen.has_value())
    graphic::fullscreen = *g_ctx.start_fullscreen;

  init_port_cbs();

  g_ctx.pixels = scast<cp<void>>(graphic::canvas.data());

#ifndef DISABLE_CONFIG
  init_archive(); // из архива понадобятся шейдеры
#endif
#ifndef ECOMEM
  load_color_tables();
#endif

  set_target_ups(hpw::target_ups);
  graphic::set_target_fps(graphic::get_target_fps());
  hpw::safe_dt = graphic::get_target_frame_time();
}

//---- Основные функции GLFW3 порта --------------------------------------------
void glfw3_init(int argc, char** argv) {
  log_debug << "GLFW3 port initialization...";

  iferror(g_ctx.window, "не использовать два окна");

  glfwSetErrorCallback(error_cb);
  iferror(!glfwInit(), "glfwInit error");

  reset_keymap();
  clear_cur_keys();

  init_port(argc, argv);
  init_window();
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
  cleanup_opengl();

  if (g_ctx.window) {
    glfwDestroyWindow(g_ctx.window);
    g_ctx.window = nullptr;
  }

  glfwTerminate();
  
#ifndef DISABLE_CONFIG
  save_config();
#endif
}
