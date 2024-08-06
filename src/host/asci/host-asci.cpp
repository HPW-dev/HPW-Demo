/*
#include <thread>
//#include <chrono>
#include <iostream>
#include <cassert>
#include <mutex>
#include <algorithm>
#include <ctime>
#include "stb/stb_image.h"
#include "host/host-util.hpp"
#include "host-glfw.hpp"
#include "host-glfw-keymap.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "util/math/mat.hpp"
#include "util/math/random.hpp"
#include "game/util/keybits.hpp"
#include "game/util/sync.hpp"
#include "game/util/game-archive.hpp"
#include "game/core/common.hpp"
#include "game/core/core.hpp"
#include "game/core/debug.hpp"
#include "game/core/core-window.hpp"
#include "game/core/graphic.hpp"
#include "game/core/user.hpp"
extern "C" {
  #include "host/ogl3/ogl.hpp"
  #ifdef WINDOWS
    #define GLFW_DLL
  #else
    #include <unistd.h>
  #endif
  #include <GLFW/glfw3.h>
}

inline std::atomic<Host_glfw*> g_instance {};
inline bool g_rebind_key_mode {false};
// позволяет избежать зацикливания при выставлении стандартной гаммы при ошибке
inline bool g_set_default_gamma_once {true};
// появится при hpw::rebind_key
inline hpw::keycode g_key_for_rebind;

static void host_glfw_set_vsync(bool enable) {
  detailed_log("vsync: " << enable << '\n');
  glfwSwapInterval(enable ? 1 : 0);
}

static void key_callback(GLFWwindow* m_window, int key, int scancode, int action, int mods) {
  hpw::any_key_pressed = true;
  nauto key_mapper = *(g_instance.load()->m_key_mapper.get());
  nauto keymap_table = key_mapper.get_table();

  // режим ребинда клавиши
  if (g_rebind_key_mode) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      key_mapper.bind(g_key_for_rebind, scancode);
      hpw::keys_info = key_mapper.get_info();
      g_rebind_key_mode = false;
      return; // чтобы нажатие не применилось в игровой логике
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
  // поставить дефолтную гамму при ошибке
  if (g_instance && g_set_default_gamma_once) {
    g_instance.load()->set_gamma(1);
    g_set_default_gamma_once = false;
  }

  error("GLFW error: " << error << ": " << description);
}

static void reshape_callback(GLFWwindow* m_window, int w, int h)
{ g_instance.load()->reshape(w, h); }

// utf32 text input callback
static void utf32_text_input_cb(GLFWwindow* m_window, std::uint32_t codepoint) {
  if (hpw::text_input_mode) {
    hpw::text_input_pos = std::clamp<int>(hpw::text_input_pos,
      0, hpw::text_input.size());
    hpw::text_input.insert(hpw::text_input_pos, 1,
      scast<decltype(hpw::text_input)::value_type>(codepoint));
    hpw::text_input_pos = std::min<int>(hpw::text_input_pos + 1,
      hpw::text_input.size());
  }
}

Host_glfw::Host_glfw(int argc, char *argv[])
: Host_ogl (argc, argv)
, m_wnd_x (480), m_wnd_y (40)
{
  iferror(g_instance, "no use two GLFW hosts");
  g_instance.store(this);

  hpw::set_vsync = &host_glfw_set_vsync;
  glfwSetErrorCallback(error_callback);
  detailed_log("init GLFW lib\n");
  iferror(!glfwInit(), "!glfwInit");

  init_window();
  init_shared(m_key_mapper);
  m_key_mapper->reset();
  hpw::keys_info = m_key_mapper->get_info();
  init_commands();
} // Host_glfw c-tor

Host_glfw::~Host_glfw() {
  glfwDestroyWindow(m_window);
  glfwTerminate();
  g_instance = {};
}

void Host_glfw::init_commands() {
  hpw::rebind_key = [](hpw::keycode hpw_key){
    g_rebind_key_mode = true;
    g_key_for_rebind = hpw_key;
  };
  hpw::rebind_key_by_scancode = [this](hpw::keycode hpw_key, int scancode){
    return_if( !m_key_mapper);
    m_key_mapper->bind(hpw_key, scancode);
    hpw::keys_info = m_key_mapper->get_info();
  };
  hpw::reset_keymap = [this]{
    m_key_mapper->reset();
    hpw::keys_info = m_key_mapper->get_info();
  };
  hpw::set_gamma = [this](const double val) { this->set_gamma(val); };
  hpw::get_time = [this]{ return this->get_time(); };
} // init_commands

void Host_glfw::init() {
  m_start_update_time = get_time();
  set_target_ups(hpw::target_ups);
  graphic::set_target_fps(graphic::get_target_fps());
  hpw::safe_dt = graphic::get_target_frame_time();
  hpw::keys_info = m_key_mapper->get_info();
}

void Host_glfw::run() {
  Host_ogl::run();
  init();
  auto last_loop_time = get_time();
  
  while (is_ran()) {
    glfwPollEvents();
    game_update(hpw::safe_dt);
    game_frame(hpw::safe_dt);

    // вычисление Delta Time
    cauto loop_time = get_time();
    cauto dt = loop_time - last_loop_time;
    last_loop_time = loop_time;
    game_set_dt(dt);
    game_set_fps_info(dt);
  }

  m_is_ran = false;
} // run

void Host_glfw::reshape(int w, int h) {
  if (!graphic::fullscreen)
    glfwSetWindowSize(m_window, w, h);
  Host_ogl::reshape(w, h);
}

void Host_glfw::set_window_pos(int x, int y) {
  glfwSetWindowPos(m_window, x, y);
  Host_ogl::set_window_pos(x, y);
}

void Host_glfw::game_set_dt(const Delta_time gameloop_time) {
  hpw::real_dt = gameloop_time;
  // ограничение чтобы игра фризила, а не обновлялась рывками
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
    glfwSetWindowMonitor(m_window, monitor, 0, 0,
      mode->width, mode->height, mode->refreshRate);
    graphic::set_vsync( graphic::get_vsync() );
  } else { // переключение обратно в окно
    glfwSetWindowMonitor(m_window, nullptr, m_wnd_x, m_wnd_y,
      m_w, m_h, GLFW_DONT_CARE);
    reshape(m_w, m_h);
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
  if (m_window) // на случай реинита
    glfwDestroyWindow(m_window);

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
  detailed_log("make m_window\n");
  m_window = glfwCreateWindow(m_w, m_h, rnd_window_name().c_str(), nullptr, nullptr);
  iferror(!m_window, "bad init GLFW m_window");
  glfwSetWindowPos(m_window, m_wnd_x, m_wnd_y);
  glfwMakeContextCurrent(m_window);

  // заливка окна при ините, чтобы показать что приложение живое
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  glFinish();
  glfwSwapBuffers(m_window);

  // подгрузка расширений OGL
  auto ver = Str(cptr2ptr<Cstr>(glGetString(GL_VERSION)));
  detailed_log("OGL version: " << ver << "\n");
  detailed_log("GLEW init\n");
  iferror(glewInit() != GLEW_OK, "GLEW init error");
  ogl_post_init();
  glfwSetWindowSizeCallback(m_window, reshape_callback);
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
  glfwSetKeyCallback(m_window, &key_callback);
  // для чтения юникод текста при вводе
  glfwSetCharCallback(m_window, &utf32_text_input_cb);
  // режим показа курсора мыши
  _set_mouse_cursour_mode(graphic::show_mouse_cursour);
  // поставить иконку окну
  init_icon();
} // init_window

Delta_time Host_glfw::get_time() const {
  return glfwGetTime();
  // static cauto _st = std::chrono::steady_clock::now();
  // cauto _ed = std::chrono::steady_clock::now();
  // using Seconds = std::chrono::duration<Delta_time, std::ratio<1, 1>>;
  // return std::chrono::duration_cast<Seconds>(_ed - _st).count();
}

void Host_glfw::game_set_fps_info(const Delta_time gameloop_time) {
  m_fps_timer += gameloop_time;
  if (m_fps_timer > 1) {
    graphic::cur_fps = safe_div(m_fps, m_fps_timer);
    hpw::cur_ups = safe_div(m_ups, m_fps_timer);
    m_fps = 0;
    m_ips = 0;
    m_ups = 0;
    m_fps_timer -= 1;
  }
}

bool Host_glfw::is_ran() const { return m_is_ran && !glfwWindowShouldClose(m_window); }

void Host_glfw::game_frame(const Delta_time dt) {
  return_if (!graphic::enable_render);
  return_if (dt <= 0 || dt >= 10);

  m_frame_time += dt;
  static auto last_frame_time = get_time();

  if (
    // риосвать кадр если пришло время для него
    m_frame_time >= graphic::get_target_frame_time() ||
    // или если нет лимита кадров
    graphic::get_disable_frame_limit()
  ) {
    m_frame_time = 0;
    check_frame_skip();
    calc_upf();
    
    if (!graphic::skip_cur_frame) { // не рисовать кадр при этом флаге
      calc_lerp_alpha();
      draw_game_frame();
      draw();
      glfwSwapBuffers(m_window);
      m_frame_drawn = true;
      apply_render_delay();
      ++m_fps;
    }
    ++graphic::frame_count;

    // вычислить сколько длился рендер игры и хоста
    cauto new_frame_time = get_time();
    graphic::hard_draw_time = new_frame_time - last_frame_time;
    last_frame_time = new_frame_time;
  } else if (graphic::cpu_safe && !graphic::get_fast_forward()) {
    frame_wait();
  }
} // game_frame

void Host_glfw::game_update(const Delta_time dt) {
  set_update_time(dt);
  return_if (dt <= 0 || dt >= 10);

  if (graphic::get_fast_forward())
    m_update_time = hpw::target_update_time * graphic::FAST_FWD_UPD_SPDUP;

  while (m_update_time >= hpw::target_update_time) {
    m_update_time -= hpw::target_update_time;
    m_start_update_time = get_time();
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
    ++m_upf;
    ++m_ups;
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
  cauto start_draw_time = get_time() - m_start_update_time;
  // для интеропляции движения 
  graphic::lerp_alpha = safe_div(start_draw_time, hpw::target_update_time);
  // лимит значения чтобы при тормозах окна объекты не растягивались
  graphic::lerp_alpha = std::clamp<Delta_time>(graphic::lerp_alpha, 0, 1);
}

void Host_glfw::calc_upf() {
  hpw::cur_upf = m_upf;
  m_upf = 0;
}

void Host_glfw::set_update_time(const Delta_time dt) {
  if (
    // ждать конца кадра
    (graphic::wait_frame && graphic::enable_render) &&
    // если используются синхронизации по VSync или лимиту кадров
    !graphic::get_disable_frame_limit() &&
    // юзать только при выключенном фреймскипе
    (graphic::frame_skip == 0 || (graphic::auto_frame_skip && !graphic::render_lag))
  ) {
    // ждать завершения отрисовки кадра
    if (m_frame_drawn) {
      m_update_time += graphic::get_vsync()
        ? graphic::get_target_vsync_frame_time()
        : graphic::get_target_frame_time();
      m_frame_drawn = false;
    }
  } else {
    m_update_time += dt;
  }
} // set_update_time

void Host_glfw::_set_mouse_cursour_mode(bool enable) {
  graphic::show_mouse_cursour = enable;
  glfwSetInputMode(m_window, GLFW_CURSOR,
    enable ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

void Host_glfw::update(const Delta_time dt) {}

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
  auto delay = graphic::get_target_frame_time() - m_frame_time - m_update_time;
  //constx Delta_time delay_timeout = 1.0 / 10.0;
  //delay = std::clamp<Delta_time>(delay, 0, delay_timeout);
  delay = std::clamp<Delta_time>(delay, 0, 1);
  delay_sec(delay);
}

void Host_glfw::init_icon() {
  GLFWimage icon;
  icon.pixels = nullptr;

  try {
    // случайно выбрать ярлык окна
    sconst Strs icon_names {
      "resource/image/icon/128x128/Alles.png",
      "resource/image/icon/128x128/boo.png",
      "resource/image/icon/128x128/pattern 1.png",
      "resource/image/icon/128x128/pattern 2.png",
      "resource/image/icon/128x128/pattern 3.png",
    };
    cauto icon_name = icon_names.at(rndu_fast(icon_names.size() - 1));
    cauto icon_file = hpw::archive->get_file(icon_name);
    int channels;
    icon.pixels = stbi_load_from_memory (
      rcast<CP<stbi_uc>>(icon_file.data.data()), icon_file.data.size(),
      &icon.width, &icon.height, &channels, 4
    );
    iferror(channels != 4, "цветовых каналов в изображении " << n2s(channels) << ", а нужно 4");
    glfwSetWindowIcon(m_window, 1, &icon); 
  } catch (CN<hpw::Error> err) {
    hpw_log("не удалось установить иконку для окна.\nОшибка: " << err.what() << "\n");
  } catch (...) {
    hpw_log("не удалось установить иконку для окна. Неизвестная ошибка\n");
  }

  stbi_image_free(icon.pixels);
} // init_icon
*/

#include "host-asci.hpp"

struct Host_asci::Impl {
  Host_asci& _master;
  int _argc {};
  char** _argv {};

  inline Impl(Host_asci& master, int argc, char** argv)
  : _master {master}
  , _argc {argc}
  , _argv {argv}
  {}

  inline Delta_time get_time() const { return 0; /*TODO*/ }
  inline void draw_game_frame() const {}
  inline void run() {}
  inline void update(const Delta_time dt) {}
};

Host_asci::Host_asci(int argc, char** argv)
: Host(argc, argv)
, impl {new_unique<Impl>(*this, argc, argv)}
{}

Host_asci::~Host_asci() {}
Delta_time Host_asci::get_time() const { return impl->get_time(); }
void Host_asci::draw_game_frame() const { impl->draw_game_frame(); }
void Host_asci::run() { impl->run(); }
void Host_asci::update(const Delta_time dt) { impl->update(dt); }
