#include "level.hpp"
#include "util/math/vec.hpp"
#include "util/math/timer.hpp"
#ifdef CLD_DEBUG
#include "game/core/debug.hpp"
#include "util/str-util.hpp"
#include "util/unicode.hpp"
#include "util/log.hpp"
#endif

struct Level::Impl {
  #ifdef CLD_DEBUG
  Timer cld_debug_timer {1};
  #endif

  Impl() = default;
  ~Impl() = default;

  inline void update(const Vec vel, double dt) {
    #ifdef CLD_DEBUG
    if (cld_debug_timer.update(dt))
      print_collision_info();
    #endif
  }

  /// дебажный вывод количества столкновений
  inline static void print_collision_info() {
    #ifdef CLD_DEBUG
    Str txt;
    txt += "circle check: " + n2s<Str>(hpw::circle_checks);
    txt += ", poly: " + n2s<Str>(hpw::poly_checks);
    txt += ", collided: " + n2s<Str>(hpw::total_collided);
    hpw_log(txt << "\n");
    hpw::circle_checks = 0;
    hpw::poly_checks = 0;
    hpw::total_collided = 0;
    #endif
  }
};

void Level::update(const Vec vel, double dt) { impl->update(vel, dt); }
Level::Level(): impl {new_unique<Impl>()} {}
Level::~Level() {}
