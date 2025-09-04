#include <cassert>
#include "scene-input-delay-test.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/util/locale.hpp"
#include "game/util/keybits.hpp"
#include "game/util/vec-helper.hpp"
#include "util/math/timer.hpp"
#include "util/math/average.hpp"
#include "util/math/random.hpp"
#include "util/platform.hpp"
#include "util/math/min-max.hpp"
#include "util/math/full-stats.hpp"
#include "graphic/font/font-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"

struct Scene_input_delay_test::Impl {
  utf32 _info_text {};
  Full_stats<uint> _frame_stats {};
  Full_stats<Delta_time> _time_stats {};
  Timer _spawn_timer {};
  constx Minmax<Delta_time> TIMER_MINMAX {.min = 3, .max = 7};
  constx uint POINT_SIZE = 10;
  Vec _point_pos {}; // позиция квадрата
  bool _show_point {}; // пора показать квадрат
  Seconds _start_time {}; // время начала замера при появлении квадрата
  mutable uint _frames {}; // сколько кадров прошло от начала замера

  inline void _reset_all() {
    _frame_stats.clear();
    _time_stats.clear();
    _frames = {};
    _start_time = {};
  }

  inline void _reset_point() {
    _spawn_timer = rndr(TIMER_MINMAX.min, TIMER_MINMAX.max);
    _point_pos = get_rand_pos_safe(50, 50, graphic::width, graphic::height);
    _show_point = false;
  }

  inline explicit Impl() {
     _info_text += get_locale_str("input_delay_test.info_1");
     _info_text += U" <" + hpw::keys_info.find(hpw::keycode::shoot)->name + U"> ";
     _info_text += get_locale_str("input_delay_test.info_2") + U". ";
     _info_text += get_locale_str("input_delay_test.info_3");
     _info_text += U" <" + hpw::keys_info.find(hpw::keycode::bomb)->name + U">\n";

    _reset_point();
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    if (is_pressed_once(hpw::keycode::bomb)) {
      _reset_point();
      _reset_all();
    }

    if (is_pressed_once(hpw::keycode::shoot) && _show_point) {
      _time_stats.push(get_cur_time() - _start_time);
      _frame_stats.push(_frames);
      _reset_point();
    }

    if (_spawn_timer.update(dt) && !_show_point) {
      _show_point = true;
      _start_time = get_cur_time();
      _frames = 0;
    }
  }

  inline void draw(Image& dst) const {
    dst.fill({});
    
    assert(graphic::font);
    text_bordered(dst, _info_text
      + U"\nTime: " + utf8_to_32(_time_stats.str(3, false))
      + U"\nFrame: " + utf8_to_32(_frame_stats.str(0, false)),
      graphic::font.get(), {5, 150, dst.X, 400}, {5, 5}
    );

    if (_show_point) {
      draw_rect_filled<&blend_diff>(dst, Rect{_point_pos, Vec{POINT_SIZE, POINT_SIZE}}, Pal8::white);
      ++_frames;
    }
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_input_delay_test)
