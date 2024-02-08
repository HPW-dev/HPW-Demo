#include <cassert>
#include "game-sync.hpp"
#include "game-graphic.hpp"
#include "game-core.hpp"
#include "host/command.hpp"
#include "util/log.hpp"

static bool m_vsync {true};
static bool m_disable_frame_limit {true};
static bool m_fast_forward {false}; /// ускорение игры
static int m_target_fps = 360;
static int m_target_vsync_fps = 60;
static double m_target_frame_time = 1.0 / m_target_fps;
static double m_target_vsync_frame_time = m_target_frame_time;
static double m_bad_fps = 60 * 0.9; /// значение, ниже которго включается оптимизация
static double m_draw_time_autoopt_limit = 1.0 / (60 * 0.9); /// лимит рендер тайма, для автооптимизации

namespace graphic {

void set_vsync(bool val) {
  m_vsync = val;

  if (hpw::set_vsync)
    hpw::set_vsync(val);
  else
    detailed_log("graphic::set_vsync был вызван без иниц-го hpw::set_vsync\n");

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

  if (m_fast_forward) { // on
    set_disable_frame_limit( bak_disable_frame_limit );
    set_vsync(bak_vsync);
    graphic::render_lag = false; // в фреймскипе игра подумает что она лагает
  } else { // off
    bak_disable_frame_limit = graphic::get_disable_frame_limit();
    bak_vsync = graphic::get_vsync();
    set_disable_frame_limit(true);
    set_vsync(false);
  }
} // set_fast_forward

void set_target_fps(int val) {
  assert(val > 0);
  // выключить VSync, если new_fps не совпадае с частотой монитора
  if (val != get_target_vsync_fps())
    set_vsync(false);
  m_target_fps = val;
  set_target_frame_time( 1.0 / m_target_fps );
  set_bad_fps( 0.9 * (get_vsync() ? get_target_vsync_fps() : val) );
}

void set_target_vsync_fps(int val) {
  assert(val > 0);
  m_target_vsync_fps = val;
  set_target_vsync_frame_time(1.0 / val);
}

void set_target_frame_time(double val) {
  assert(val > 0);
  assert(val <= 10);
  m_target_frame_time = val;
}

void set_target_vsync_frame_time(double val) {
  assert(val > 0);
  assert(val < 10);
  m_target_vsync_frame_time = val;
}

void set_bad_fps(double val) {
  assert(val >= 0);
  m_bad_fps = val;
  set_draw_time_autoopt_limit(1.0 / get_bad_fps());
}

void set_draw_time_autoopt_limit(double val) {
  assert(val >= 0);
  assert(val < 10);
  m_draw_time_autoopt_limit = val;
}

void check_autoopt() {
  cauto autoopt_trigger = hpw::real_dt >= graphic::get_draw_time_autoopt_limit();
  if (autoopt_trigger) {
    graphic::render_lag = true;
    graphic::autoopt_timeout = graphic::autoopt_timeout_max;
  }
}

bool get_vsync() { return m_vsync; }
bool get_disable_frame_limit() { return m_disable_frame_limit; }
bool get_fast_forward() { return m_fast_forward; }
int get_target_fps() { return m_target_fps; }
int get_target_vsync_fps() { return m_target_vsync_fps; }
double get_target_frame_time() { return m_target_frame_time; }
double get_target_vsync_frame_time() { return m_target_vsync_frame_time; }
double get_bad_fps() { return m_bad_fps; }
double get_draw_time_autoopt_limit() { return m_draw_time_autoopt_limit; }

} // graphic ns
