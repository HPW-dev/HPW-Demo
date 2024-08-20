#include <cassert>
#include <chrono>
#include <cstdlib>
#include "host-asci.hpp"
#include "game/core/core.hpp"
#include "game/core/canvas.hpp"
#include "game/core/graphic.hpp"
#include "game/core/common.hpp"
#include "game/core/debug.hpp"
#include "game/util/keybits.hpp"
#include "game/util/sync.hpp"
#include "game/util/game-archive.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/pparser.hpp"
#include "util/math/mat.hpp"
#include "util/math/random.hpp"
#include "host/host-util.hpp"
#include "graphic/effect/image-to-asci.hpp"

// -------------- если переместить этот хедевр вверх - всё взорвётся! ------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct Host_asci::Impl {
  Host_asci& m_master;
  int m_argc {};
  char** m_argv {};
  Delta_time m_fps_timer {}; // для высчитывания фпс
  uint m_fps {};
  uint m_ups {};
  uint m_upf {};
  uint m_ips {};
  Delta_time m_frame_time {};
  Delta_time m_update_time {};
  Delta_time m_start_update_time {}; // нужен для интерполяции движения
  bool m_frame_drawn {}; // для плавного апдейта игры

  uint m_console_w = 80; // разрешение консоли по ширине (в символах)
  uint m_console_h = 24; // разрешение консоли по высоте (в символах)
  Img_to_asci_mode m_asci_pal {Img_to_asci_mode::grayscale_large_pal};
  uint m_target_fps {15};
  uint m_frameskip {3};

  inline Impl(Host_asci& master, int argc, char** argv)
  : m_master {master}
  , m_argc {argc}
  , m_argv {argv}
  {
    parse_args();
    init_commands();
    init_core();
    init_console();
    init_input();
  }

  inline ~Impl() { clear_console(); }

  inline Delta_time get_time() const {
    static cauto _st = std::chrono::steady_clock::now();
    cauto _ed = std::chrono::steady_clock::now();
    using Seconds = std::chrono::duration<Delta_time, std::ratio<1, 1>>;

    return std::chrono::duration_cast<Seconds>(_ed - _st).count();
  }

  inline void draw_frame() const {
    assert(graphic::canvas);
    auto asci = to_asci(*graphic::canvas, m_console_w, m_console_h, m_asci_pal);
    remove_all(asci, '\n'); // не рендерить символы переноса
    remove_all(asci, '\r'); // не рендерить символы переноса
    
    // быстрый рендер в консоль
    auto hOutput = rcast<HANDLE>( GetStdHandle(STD_OUTPUT_HANDLE) );
    assert(hOutput);

    assert(m_console_w-1 > 2);
    assert(m_console_h-1 > 2);
    Vector<CHAR_INFO> buffer(m_console_w * m_console_h);
    COORD dwBufferSize (m_console_w, m_console_h);
    constexpr COORD dwBufferCoord (0, 0);
    SMALL_RECT rcRegion (0, 0,  m_console_w-1,  m_console_h-1);
    
    ReadConsoleOutput(hOutput, buffer.data(), dwBufferSize, dwBufferCoord, &rcRegion);
    cfor (i, m_console_w * m_console_h) {
      buffer[i].Char.AsciiChar = asci.at(i);
      buffer[i].Attributes = 0x0E;
    }
    WriteConsoleOutput(hOutput, buffer.data(), dwBufferSize, dwBufferCoord, &rcRegion);
  }

  inline void run() {
    game_init();
    auto last_loop_time = get_time();
  
    while (m_master.m_is_ran) {
      input_update();
      game_update(hpw::safe_dt);
      game_frame(hpw::safe_dt);

      // вычисление Delta Time
      cauto loop_time = get_time();
      cauto dt = loop_time - last_loop_time;
      last_loop_time = loop_time;
      game_set_dt(dt);
      game_set_fps_info(dt);
    }

    m_master.m_is_ran = false;
  }

  inline void init_core() {
    init_archive();
    load_color_tables();
    set_target_ups(hpw::target_ups);
    // Graphic:
    hpw::set_resize_mode(Resize_mode::one_to_one);
    hpw::set_fullscreen(false);
    assert(m_console_h > 4);
    assert(m_console_w > 3);
    graphic::set_target_vsync_fps(m_target_fps);
    graphic::set_target_fps(m_target_fps);
    graphic::frame_skip = m_frameskip;
    // Canvas:
    init_unique(graphic::canvas, graphic::width, graphic::height);
    iferror(graphic::canvas->size == 0 || graphic::canvas->size >= 1024*720,
      "canvas bad size: " + n2s(graphic::canvas->size));
  }

  inline void init_console() {
    clear_console();
  }

  inline void clear_console() { system("cls"); }

  inline void init_commands() {
    hpw::set_vsync = [](bool){ detailed_log("настройка VSync в ASCI режиме ни на что не влияет\n"); };
    hpw::rebind_key = [](hpw::keycode) { error("rebind_key not implemented for ASCI-host"); };
    hpw::reset_keymap = []{ { error("reset_keymap not implemented for ASCI-host"); }; };
    hpw::set_gamma = [](const double) { detailed_log("изменение гаммы ни на что не влияет в ASCI-режиме\n") };
    hpw::get_time = [this]{ return this->get_time(); };
    hpw::set_fullscreen = [](bool) { detailed_log("настройка fullscreen в ASCI режиме ни на что не влияет\n"); };
    hpw::set_double_buffering = [](bool) { detailed_log("настройка double buffering в ASCI режиме ни на что не влияет\n"); };
    hpw::set_resize_mode = [](Resize_mode) { detailed_log("настройка resize mode в ASCI режиме ни на что не влияет\n"); };
    hpw::set_mouse_cursour_mode = [](bool) { detailed_log("в ASCI режиме настройки курсора мыши не применяются\n"); };
    
    hpw::rebind_key_by_scancode = [this](hpw::keycode hpw_key, int scancode) {
      // TODO
      detailed_log("Warning: rebind_key_by_scancode not implemented for ASCI-host\n");
    };
  }

  inline void init_input() {
    //init_shared(m_key_mapper); from GLFW
    //m_key_mapper->reset();
    //hpw::keys_info = m_key_mapper->get_info();

    hpw::keys_info = Keys_info {
      .keys = {
        #define KEY_DEF(HPW_KEY, TITLE, SCANCODE) \
        Keys_info::Item {.hpw_key=hpw::keycode::HPW_KEY, .name=TITLE, .scancode=SCANCODE},
        KEY_DEF(up,           U"Arrow Up",    38)
        KEY_DEF(down,         U"Arrow Down",  40)
        KEY_DEF(left,         U"Arrow Left",  37)
        KEY_DEF(right,        U"Arrow Right", 39)
        KEY_DEF(focus,        U"Left Alt",    164)
        KEY_DEF(shoot,        U"S",           83)
        KEY_DEF(bomb,         U"X",           88)
        KEY_DEF(mode,         U"A",           65)
        KEY_DEF(escape,       U"Escape",      27)
        KEY_DEF(enable,       U"Enter",       13)
        KEY_DEF(reset,        U"F5",          116)
        KEY_DEF(fast_forward, U"F4",          115)
        KEY_DEF(debug,        U"F3",          114)
        KEY_DEF(console,      U"~",           192)
        KEY_DEF(text_delete,  U"BackSpace",   8)
        KEY_DEF(fps,          U"F1",          112)
        KEY_DEF(fulscrn,      U"F11",         122)
        KEY_DEF(screenshot,   U"F2",          113)
        #undef KEY_DEF
      }
    };
  }

  inline void game_update(const Delta_time dt) {
    set_update_time(dt);
    return_if (dt <= 0 || dt >= 10);

    if (graphic::get_fast_forward())
      m_update_time = hpw::target_update_time * graphic::FAST_FWD_UPD_SPDUP;

    while (m_update_time >= hpw::target_update_time) {
      m_update_time -= hpw::target_update_time;
      m_start_update_time = get_time();

      // обработка специальных кнопок
      if (is_pressed_once(hpw::keycode::screenshot))
        hpw::make_screenshot();
      hpw::any_key_pressed |= is_any_key_pressed();

      // обновить игровое состояние
      if (graphic::step_mode) { // пошагово
        if (hpw::any_key_pressed)
          m_master.update(hpw::target_update_time);
      } else { // каждый раз
        m_master.update(hpw::target_update_time);
      }

      hpw::any_key_pressed = false;
      keys_cur_to_prev();
      ++m_upf;
      ++m_ups;
      apply_update_delay();
    } // while update time
  }

  inline void game_frame(const Delta_time dt) {
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
        draw_frame();
        m_master.draw_game_frame();
        m_frame_drawn = true;
        apply_render_delay();
        ++m_fps;
      }
      ++graphic::frame_count;

      // вычислить сколько длился рендер игры и хоста
      cauto new_frame_time = get_time();
      graphic::hard_draw_time = new_frame_time - last_frame_time;
      last_frame_time = new_frame_time;
    } elif (graphic::cpu_safe && !graphic::get_fast_forward()) {
      frame_wait();
    }
  }

  inline void game_set_dt(const Delta_time dt) {
    hpw::real_dt = dt;
    // ограничение чтобы игра фризила, а не обновлялась рывками
    hpw::safe_dt = std::clamp(dt, 0.000001, 1.0 / (60 * 0.9));
    graphic::effect_state = std::fmod(graphic::effect_state + hpw::real_dt, 1.0);
  }

  inline void game_set_fps_info(const Delta_time dt) {
    m_fps_timer += dt;
    if (m_fps_timer > 1) {
      graphic::cur_fps = safe_div(m_fps, m_fps_timer);
      hpw::cur_ups = safe_div(m_ups, m_fps_timer);
      m_fps = 0;
      m_ips = 0;
      m_ups = 0;
      m_fps_timer -= 1;
    }
  }

  inline void game_init() {
    m_start_update_time = get_time();
    hpw::safe_dt = graphic::get_target_frame_time();
    //hpw::keys_info = m_key_mapper->get_info(); TODO после чтения конфига эта инфа меняется
  }

  inline void input_update() {
    // TODO реализация utf32_text_input_cb

    hpw::any_key_pressed = false;

    // режим ребинда клавиши
    // TODO:
    /*if (g_rebind_key_mode) {
      if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        key_mapper.bind(g_key_for_rebind, scancode);
        hpw::keys_info = key_mapper.get_info();
        g_rebind_key_mode = false;
        return; // чтобы нажатие не применилось в игровой логике
      }
    }*/

    // проверить нажатия на игровые клавиши
    assert(!hpw::keys_info.keys.empty());
    for (crauto it: hpw::keys_info.keys) {
      if (GetKeyState(it.scancode) & 0x8000) {
        press(it.hpw_key);
        hpw::any_key_pressed = true;
      } else {
        release(it.hpw_key);
      }
    }

    /*// альтернативная кнопка скриншота
    if (action == GLFW_PRESS && key == GLFW_KEY_PRINT_SCREEN)
      hpw::make_screenshot();
    // альтернативная кнопка фуллскрина
    if (action == GLFW_PRESS && key == GLFW_KEY_ENTER && mods == GLFW_MOD_ALT)
      hpw::set_fullscreen( !graphic::fullscreen);*/
  }

  inline void set_update_time(const Delta_time dt) {
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

  inline void apply_update_delay() {
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

  inline void check_frame_skip() {
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
  }

  inline void calc_upf() {
    hpw::cur_upf = m_upf;
    m_upf = 0;
  }

  inline void calc_lerp_alpha() {
    cauto start_draw_time = get_time() - m_start_update_time;
    // для интеропляции движения 
    graphic::lerp_alpha = safe_div(start_draw_time, hpw::target_update_time);
    // лимит значения чтобы при тормозах окна объекты не растягивались
    graphic::lerp_alpha = std::clamp<Delta_time>(graphic::lerp_alpha, 0, 1);
  }

  inline void apply_render_delay() {
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

  inline void frame_wait() {
    // ожидание для v-sync
    auto delay = graphic::get_target_frame_time() - m_frame_time - m_update_time;
    //constx Delta_time delay_timeout = 1.0 / 10.0;
    //delay = std::clamp<Delta_time>(delay, 0, delay_timeout);
    delay = std::clamp<Delta_time>(delay, 0, 1);
    delay_sec(delay);
  }

  inline void parse_args() {
    Pparser arg_parser({
      Pparser::Param {{"-w", "--width"}, "screen width", [this](cr<Str> param) { m_console_w = s2n<uint>(param); }},
      Pparser::Param {{"-h", "--height"}, "screen height", [this](cr<Str> param) { m_console_h = s2n<uint>(param); }},
      Pparser::Param {{"-f", "--target-fps"}, "target FPS", [this](cr<Str> param) { m_target_fps = s2n<uint>(param); }},
      Pparser::Param {{"-s", "--frameskip"}, "frameskip value", [this](cr<Str> param) { m_frameskip = s2n<uint>(param); }},
      Pparser::Param {{"-p", "--asci-palette"}, "predefined asci-palettes [1, 2, 3]", [this](cr<Str> param) {
        const uint palette_index = s2n<uint>(param);
        switch (palette_index) {
          default:
          case 1: m_asci_pal = Img_to_asci_mode::grayscale_large_pal; break;
          case 2: m_asci_pal = Img_to_asci_mode::grayscale_small_pal; break;
          case 3: m_asci_pal = Img_to_asci_mode::grayscale_small_pal_2; break;
        }
      }},
    });
    arg_parser.skip_empty = true;
    arg_parser.print_info();
    arg_parser(m_argc, m_argv);
  }
}; // Impl

Host_asci::Host_asci(int argc, char** argv)
: Host(argc, argv)
, impl {new_unique<Impl>(*this, argc, argv)}
{}

Host_asci::~Host_asci() {}
Delta_time Host_asci::get_time() const { return impl->get_time(); }

void Host_asci::run() {
  Host::run();
  impl->run();
}
