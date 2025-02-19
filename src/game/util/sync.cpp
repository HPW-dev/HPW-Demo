#include <cassert>
#include "sync.hpp"
#include "game/core/graphic.hpp"
#include "game/core/core.hpp"
#include "host/command.hpp"
#include "util/log.hpp"
#include "util/platform.hpp"

static bool m_vsync {false};
static bool m_disable_frame_limit {false};
static bool m_fast_forward {false}; // ускорение игры
static int m_target_fps = 360;
static int m_target_vsync_fps = 60;
static Delta_time m_target_frame_time = 1.0 / m_target_fps;
static Delta_time m_target_vsync_frame_time = m_target_frame_time;
static Delta_time m_bad_fps = 60 * 0.9; // значение, ниже которго включается оптимизация
static Delta_time m_draw_time_autoopt_limit = 1.0 / (60 * 0.9); // лимит рендер тайма, для автооптимизации

namespace graphic {

void set_vsync(bool val) {
  m_vsync = val;

  if (hpw::set_vsync)
    hpw::set_vsync(val);
  else
    log_warning << "graphic::set_vsync был вызван без иниц-го hpw::set_vsync";

  if (val) {
    set_disable_frame_limit(false);
    set_target_fps( get_target_vsync_fps() );
  }

  // для корректного срабатывания автооптимизации при высоких фпс
  set_bad_fps( get_target_vsync_fps() * 0.9 ); 
}

void set_disable_frame_limit(bool val) {
  m_disable_frame_limit = val;

  if (val)
    set_vsync(false);
}

void set_fast_forward(bool val) {
  m_fast_forward = val;

  static auto bak_disable_frame_limit = graphic::get_disable_frame_limit();
  static auto bak_vsync = graphic::get_vsync();
  static auto bak_fps_limit = graphic::get_target_fps();
  static auto bak_motion_blur_mode = graphic::motion_blur_mode;
  static auto bak_blur_mode = graphic::blur_mode;
  static auto bak_light_quality = graphic::light_quality;
  static auto bak_heat_distort_mode = graphic::heat_distort_mode;

  if (m_fast_forward) { // on
    bak_disable_frame_limit = graphic::get_disable_frame_limit();
    bak_vsync = graphic::get_vsync();
    bak_fps_limit = graphic::get_target_fps();
    bak_motion_blur_mode = graphic::motion_blur_mode;
    bak_blur_mode = graphic::blur_mode;
    bak_light_quality = graphic::light_quality;
    bak_heat_distort_mode = graphic::heat_distort_mode;

    set_disable_frame_limit(false);
    set_target_fps(25);
    graphic::render_lag = false; // в фреймскипе игра подумает что она лагает
    graphic::motion_blur_mode = Motion_blur_mode::disabled;
    graphic::blur_mode = Blur_mode::low;
    graphic::light_quality = Light_quality::disabled;
    graphic::heat_distort_mode = Heat_distort_mode::disabled;
  } else { // off
    set_vsync(bak_vsync);
    set_target_fps(bak_fps_limit);
    set_disable_frame_limit(bak_disable_frame_limit);
    graphic::motion_blur_mode = bak_motion_blur_mode;
    graphic::blur_mode = bak_blur_mode;
    graphic::light_quality = bak_light_quality;
    graphic::heat_distort_mode = bak_heat_distort_mode;
  }
}

void set_target_fps(int val) {
  assert(val > 0);
  // выключить VSync, если new_fps не совпадае с частотой монитора
  if (val != get_target_vsync_fps())
    set_vsync(false);
  m_target_fps = val;
  set_target_frame_time( 1.0 / m_target_fps );
  set_bad_fps( 0.9 * (get_vsync() ? get_target_vsync_fps() : val) );
  // при таком низком фпс, ожидание кадра просто заставляет всё лагать
  if (val < 30)
    graphic::wait_frame_bak = graphic::wait_frame = false;
}

void set_target_vsync_fps(int val) {
  assert(val > 0);
  m_target_vsync_fps = val;
  set_target_vsync_frame_time(1.0 / val);
  #ifndef DEBUG
    calibrate_delay(1.0 / val);
  #endif
}

void set_target_frame_time(const Delta_time val) {
  assert(val > 0);
  assert(val <= 10);
  m_target_frame_time = val;
}

void set_target_vsync_frame_time(const Delta_time val) {
  assert(val > 0);
  assert(val < 10);
  m_target_vsync_frame_time = val;
}

void set_bad_fps(const Delta_time val) {
  assert(val >= 0);
  m_bad_fps = val;
  set_draw_time_autoopt_limit(1.0 / get_bad_fps());
}

void set_draw_time_autoopt_limit(const Delta_time val) {
  assert(val >= 0);
  assert(val < 10);
  m_draw_time_autoopt_limit = val;
}

void check_autoopt() {
  cauto autoopt_trigger = hpw::real_dt >= graphic::get_draw_time_autoopt_limit();

  if (autoopt_trigger) {
    graphic::render_lag = true;
    graphic::wait_frame_bak = graphic::wait_frame;
    graphic::wait_frame = false;
    graphic::autoopt_timeout = graphic::autoopt_timeout_max;
  } else {
    graphic::wait_frame = graphic::wait_frame_bak;
  }
}

bool get_vsync() { return m_vsync; }
bool get_disable_frame_limit() { return m_disable_frame_limit; }
bool get_fast_forward() { return m_fast_forward; }
int get_target_fps() { return m_target_fps; }
int get_target_vsync_fps() { return m_target_vsync_fps; }
Delta_time get_target_frame_time() { return m_target_frame_time; }
Delta_time get_target_vsync_frame_time() { return m_target_vsync_frame_time; }
Delta_time get_bad_fps() { return m_bad_fps; }
Delta_time get_draw_time_autoopt_limit() { return m_draw_time_autoopt_limit; }

} // graphic ns
