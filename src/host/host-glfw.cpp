#include <thread>
#include <iostream>
#include <cassert>
#include <mutex>
#include <algorithm>
#include <ctime>
#include "host-util.hpp"
#include "host-glfw.hpp"
#include "host-glfw-keymap.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "util/math/mat.hpp"
#include "game/util/keybits.hpp"
#include "game/util/sync.hpp"
#include "game/core/common.hpp"
#include "game/core/core.hpp"
#include "game/core/debug.hpp"
#include "game/core/core-window.hpp"
#include "game/core/graphic.hpp"
extern "C" {
  #include "ogl.hpp"
  #ifdef WINDOWS
    #define GLFW_DLL
  #else
    #include <unistd.h>
  #endif
  #include <GLFW/glfw3.h>
}

inline std::atomic<Host_glfw*> instance {};
inline bool rebind_key_mode {false};
inline hpw::keycode key_for_rebind; // появится при hpw::rebind_key

static void host_glfw_set_vsync(bool enable) {
  detailed_log("vsync: " << enable << '\n');
  glfwSwapInterval(enable ? 1 : 0);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  hpw::any_key_pressed = true;
  nauto key_mapper = *(instance.load()->key_mapper.get());
  nauto keymap_table = key_mapper.get_table();

  // режим ребинда клавиши
  if (rebind_key_mode) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      key_mapper.bind(key_for_rebind, scancode);
      hpw::keys_info = key_mapper.get_info();
      rebind_key_mode = false;
      return; // чтобы нажатие не применилось на игре
    }
  }

  // проверить нажатия на игровые клавиши
  for (cnauto [hpw_key, key]: keymap_table) {
    if (key.scancode == scancode) {
      if (action == GLFW_PRESS || action == GLFW_REPEAT)
        press(hpw_key);
      else // GLFW_RELEASE
        release(hpw_key);
    }
  }

  // альтернативная кнопка скриншота
  if (action == GLFW_PRESS && key == GLFW_KEY_PRINT_SCREEN)
    hpw::make_screenshot();
  // альтернативная кнопка фуллскрина
  if (action == GLFW_PRESS && key == GLFW_KEY_ENTER && mods == GLFW_MOD_ALT)
    hpw::set_fullscreen( !graphic::fullscreen);
} // key_callback

// колбэк для ошибок нужен для GLFW
static void error_callback(int error, Cstr description) {
  // на всякий случай вернуть гамму как была
  auto monitor = glfwGetPrimaryMonitor();
  glfwSetGamma(monitor, 1.0);
  error("GLFW error: " << error << ": " << description);
}

static void reshape_callback(GLFWwindow* /*window*/, int w, int h)
{ instance.load()->reshape(w, h); }

Host_glfw::Host_glfw(int argc, char *argv[])
: Host_ogl (argc, argv)
, wnd_x (480), wnd_y (40)
{
  iferror(instance, "no use two GLFW hosts");
  instance.store(this);

  hpw::set_vsync = &host_glfw_set_vsync;
  glfwSetErrorCallback(error_callback);
  detailed_log("init GLFW lib\n");
  iferror(!glfwInit(), "!glfwInit");

  init_window();
  key_mapper = new_shared<Key_mapper>();
  key_mapper->reset();
  hpw::keys_info = key_mapper->get_info();
  init_commands();
} // Host_glfw c-tor

Host_glfw::~Host_glfw() {
  glfwDestroyWindow(window);
  glfwTerminate();
  instance = {};
}

void Host_glfw::init_commands() {
  hpw::rebind_key = [](hpw::keycode hpw_key){
    rebind_key_mode = true;
    key_for_rebind = hpw_key;
  };
  hpw::rebind_key_by_scancode = [this](hpw::keycode hpw_key, int scancode){
    return_if( !key_mapper);
    key_mapper->bind(hpw_key, scancode);
    hpw::keys_info = key_mapper->get_info();
  };
  hpw::reset_keymap = [this]{
    key_mapper->reset();
    hpw::keys_info = key_mapper->get_info();
  };
  hpw::set_gamma = [this](const double val) { this->set_gamma(val); };
} // init_commands

void Host_glfw::init() {
  start_update_time = get_time();
  set_target_ups(hpw::target_ups);
  graphic::set_target_fps(graphic::get_target_fps());
  hpw::safe_dt = graphic::get_target_frame_time();
  hpw::keys_info = key_mapper->get_info();
}

void Host_glfw::run() {
  Host_ogl::run();
  init();
  
  while (is_ran()) {
    auto gameloop_time_point_start = get_time();

    glfwPollEvents();
    game_update(hpw::safe_dt);
    game_frame(hpw::safe_dt);

    auto gameloop_time_point_end = get_time();
    auto gameloop_time = gameloop_time_point_end - gameloop_time_point_start;
    game_set_dt(gameloop_time);
    game_set_fps_info(gameloop_time);
  } // while is_ran

  is_run = false;
} // run

void Host_glfw::reshape(int w, int h) {
  glfwSetWindowSize(window, w, h);
  Host_ogl::reshape(w, h);
}

void Host_glfw::set_window_pos(int x, int y) {
  glfwSetWindowPos(window, x, y);
  Host_ogl::set_window_pos(x, y);
}

void Host_glfw::game_set_dt(double gameloop_time) {
  hpw::real_dt = gameloop_time;
  hpw::safe_dt = std::clamp(gameloop_time, 0.000001, 1.0 / (60 * 0.9));
  graphic::effect_state = std::fmod(graphic::effect_state + hpw::real_dt, 1.0);
}

void Host_glfw::_set_fullscreen(bool enable) {
  detailed_log("fullscreen mode: " << std::boolalpha << enable << '\n');
  graphic::fullscreen = enable;
  
  if (enable) {
    auto monitor = glfwGetPrimaryMonitor();
    auto mode = glfwGetVideoMode(monitor);
    reshape(mode->width, mode->height);
    glfwSetWindowMonitor(window, monitor, 0, 0,
      mode->width, mode->height, mode->refreshRate);
    graphic::set_vsync( graphic::get_vsync() );
  } else { // переключение обратно в окно
    glfwSetWindowMonitor(window, nullptr, wnd_x, wnd_y,
      w_, h_, GLFW_DONT_CARE);
    reshape(w_, h_);
    graphic::set_vsync( graphic::get_vsync() );
  }
} // _set_fullscreen

void Host_glfw::_set_double_buffering(bool enable) {
  detailed_log("Host_glfw._set_double_buffering: " << enable << "\n");
  graphic::double_buffering = enable;
  init_window();
}

void Host_glfw::set_gamma(const double gamma) {
  graphic::gamma = std::clamp<double>(gamma, 0.025, 3);
  auto monitor = glfwGetPrimaryMonitor();
  glfwSetGamma(monitor, graphic::gamma);
}

void Host_glfw::init_window() {
  if (window) // на случай реинита
    glfwDestroyWindow(window);

  // настройки для контекста OGL
  glfwWindowHint(GLFW_DEPTH_BITS, 0);
  glfwWindowHint(GLFW_STENCIL_BITS, 0);
  glfwWindowHint(GLFW_ALPHA_BITS, 0);
  glfwWindowHint(GLFW_ACCUM_RED_BITS, 0);
  glfwWindowHint(GLFW_ACCUM_GREEN_BITS, 0);
  glfwWindowHint(GLFW_ACCUM_BLUE_BITS, 0);
  glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, 0);
  glfwWindowHint(GLFW_AUX_BUFFERS, 0);
  glfwWindowHint(GLFW_STEREO, 0);
  glfwWindowHint(GLFW_SAMPLES, 0);
  glfwWindowHint(GLFW_SRGB_CAPABLE, 0);
  // выбор версии OGL
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  if (graphic::start_focused) {
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
  }
  // вкл/выкл даблбуфер
  glfwWindowHint (
    GLFW_DOUBLEBUFFER,
    graphic::double_buffering
      ? GL_TRUE
      : GL_FALSE
  ); 
  detailed_log("make window\n");
  window = glfwCreateWindow(w_, h_, "H.P.W", nullptr, nullptr);
  iferror(!window, "bad init GLFW window");
  glfwSetWindowPos(window, wnd_x, wnd_y);
  glfwMakeContextCurrent(window);

  // заливка окна при ините, чтобы показать что приложение живое
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  glFinish();
  glfwSwapBuffers(window);

  // подгрузка расширений OGL
  auto ver = Str(cptr2ptr<Cstr>(glGetString(GL_VERSION)));
  detailed_log("OGL version: " << ver << "\n");
  detailed_log("GLEW init\n");
  iferror(glewInit() != GLEW_OK, "GLEW init error");
  ogl_post_init();
  glfwSetWindowSizeCallback(window, reshape_callback);
  hpw::set_resize_mode(graphic::resize_mode);
  hpw::set_fullscreen(graphic::fullscreen);

  // поставить лимит фпс в 2 раза больше чем на экране
  auto monitor = glfwGetPrimaryMonitor();
  assert(monitor);
  auto monitor_mode = glfwGetVideoMode(monitor);
  assert(monitor_mode);
  
  // подстрока по частоте монитора
  auto monitor_framerate = monitor_mode->refreshRate;
  if (monitor_framerate > 0)
    graphic::set_target_vsync_fps(monitor_framerate);
  if (graphic::get_target_fps() == 0)
    graphic::set_target_fps(std::max(monitor_framerate * 2, 90));
  // определить растягивание окна по умолчанию
  if (monitor_mode->width > 0 && monitor_mode->height > 0) {
    if (monitor_mode->width == monitor_mode->height)
      graphic::default_resize_mode = Resize_mode::one_to_one;
    else if (monitor_mode->width > monitor_mode->height)
      graphic::default_resize_mode = Resize_mode::by_height;
    else
      graphic::default_resize_mode = Resize_mode::by_width;
  }

  // ивенты клавы
  glfwSetKeyCallback(window, &key_callback);
  // режим показа курсора мыши
  _set_mouse_cursour_mode(graphic::show_mouse_cursour);
} // init_window

double Host_glfw::get_time() const { return glfwGetTime(); }

void Host_glfw::game_set_fps_info(double gameloop_time) {
  second_timer += gameloop_time;
  if (second_timer > 1) {
    graphic::cur_fps = safe_div(fps, second_timer);
    hpw::cur_ups = safe_div(ups, second_timer);
    fps = 0;
    ips = 0;
    ups = 0;
    second_timer -= 1;
  }
}

bool Host_glfw::is_ran() const { return is_run && !glfwWindowShouldClose(window); }

void Host_glfw::game_frame(double dt) {
  return_if (dt <= 0 || dt >= 10);

  frame_time += dt;

  if (
    frame_time >= graphic::get_target_frame_time() ||
    graphic::get_disable_frame_limit()
  ) {
    auto frame_draw_start = get_time();
    frame_time = 0;
    check_frame_skip();
    calc_upf();
    
    if (!graphic::skip_cur_frame) { // не рисовать кадр при этом флаге
      calc_lerp_alpha();
      draw_game_frame();
      draw();
      glfwSwapBuffers(window);
      frame_drawn = true;
      apply_render_delay();
      ++fps;
    }
    ++graphic::frame_count;

    auto frame_draw_end = get_time();
    graphic::hard_draw_time = frame_draw_end - frame_draw_start;
  } else if (graphic::cpu_safe && !graphic::get_fast_forward()) {
    frame_wait();
  }
} // game_frame

void Host_glfw::game_update(double dt) {
  set_update_time(dt);
  return_if (dt <= 0 || dt >= 10);

  if (graphic::get_fast_forward())
    update_time = hpw::target_update_time * graphic::FAST_FWD_UPD_SPDUP;

  while (update_time >= hpw::target_update_time) {
    update_time -= hpw::target_update_time;
    start_update_time = get_time();
    glfwPollEvents();

    // обработка специальных кнопок
    if (is_pressed_once(hpw::keycode::fulscrn))
      hpw::set_fullscreen(!graphic::fullscreen);
    if (is_pressed_once(hpw::keycode::screenshot))
      hpw::make_screenshot();
    hpw::any_key_pressed |= is_any_key_pressed();

    // обновить игровое состояние
    if (graphic::step_mode) { // пошагово
      if (hpw::any_key_pressed)
        update(hpw::target_update_time);
    } else { // каждый раз
      update(hpw::target_update_time);
    }

    hpw::any_key_pressed = false;
    keys_cur_to_prev();
    ++upf;
    ++ups;
    apply_update_delay();
  } // while update time
} // game_update

void Host_glfw::apply_render_delay() {
  #ifdef DEBUG
  if (hpw::render_delay) {
    graphic::render_lag = true;
    graphic::autoopt_timeout = graphic::autoopt_timeout_max;
    #ifdef WINDOWS
    Sleep(200);
    #else
    usleep(200);
    #endif
  }
  #endif
}

void Host_glfw::apply_update_delay() {
  #ifdef DEBUG
  if (hpw::update_delay) {
    #ifdef WINDOWS
    Sleep(5);
    #else
    usleep(5);
    #endif
  }
  #endif
}

void Host_glfw::calc_lerp_alpha() {
  auto start_draw_time = get_time() - start_update_time;
  // для интеропляции движения 
  graphic::lerp_alpha = safe_div(start_draw_time, hpw::target_update_time);
  // лимит значения чтобы при тормозах окна объекты не растягивались
  graphic::lerp_alpha = std::clamp<double>(graphic::lerp_alpha, 0, 1);
}

void Host_glfw::calc_upf() {
  hpw::cur_upf = upf;
  upf = 0;
}

void Host_glfw::set_update_time(double dt) {
  if (
    // ждать конца кадра
    graphic::wait_frame &&
    // если используются синхронизации по VSync или лимиту кадров
    !graphic::get_disable_frame_limit() &&
    // юзать только при выключенном фреймскипе
    (graphic::frame_skip == 0 || (graphic::auto_frame_skip && !graphic::render_lag))
  ) {
    // ждать завершения отрисовки кадра
    if (frame_drawn) {
      update_time += graphic::get_vsync()
        ? graphic::get_target_vsync_frame_time()
        : graphic::get_target_frame_time();
      frame_drawn = false;
    }
  } else {
    update_time += dt;
  }
} // set_update_time

void Host_glfw::_set_mouse_cursour_mode(bool enable) {
  graphic::show_mouse_cursour = enable;
  glfwSetInputMode(window, GLFW_CURSOR,
    enable ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

void Host_glfw::update(double dt) {}

void Host_glfw::check_frame_skip() {
  graphic::skip_cur_frame = false;

  // скип через фаст форвард
  if (graphic::get_fast_forward()) {
    graphic::skip_cur_frame = (graphic::frame_count % (1 + graphic::FAST_FWD_FRAMESKIP) == 0);
    graphic::render_lag = true; // для доп. ускорения рендера
  }

  // настраиваемый фреймскип
  if (graphic::frame_skip > 0) {
    const bool skip_me = (graphic::frame_count % (1 + graphic::frame_skip)) != 0;
    if (graphic::auto_frame_skip) { // скип при лагах
      if (graphic::render_lag)
        graphic::skip_cur_frame = skip_me;
    } else { // постоянный скип
      graphic::skip_cur_frame = skip_me;
    }
  }
} // check_frame_skip

void Host_glfw::frame_wait() {
  // ожидание для v-sync
  auto delay = graphic::get_target_frame_time() - frame_time - update_time;
  constx double delay_timeout = 1.0 / 10.0;
  delay = std::clamp<double>(delay, 0, delay_timeout);
  glfwWaitEventsTimeout(delay);
}
