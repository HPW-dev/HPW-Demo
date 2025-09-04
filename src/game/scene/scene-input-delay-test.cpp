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
#include "graphic/font/font-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"

struct Scene_input_delay_test::Impl {
  utf32 _info_text {};
  Average<Delta_time, 10'000> _avr {};
  Median<Delta_time, 10'000> _med {};
  Delta_time _last {};
  Average<uint, 10'000> _avr_frames {};
  Median<uint, 10'000> _med_frames {};
  uint _min_frames {9999};
  uint _max_frames {};
  uint _last_frames {};
  Timer _spawn_timer {};
  constx Delta_time TIMER_MIN = 3;
  constx Delta_time TIMER_MAX = 7;
  constx uint POINT_SIZE = 10;
  Vec _point_pos {}; // позиция квадрата
  bool _show_point {}; // пора показать квадрат
  Seconds _start_time {}; // время начала замера при появлении квадрата
  Seconds _min_time {9999};
  Seconds _max_time {};
  mutable uint _frames {}; // сколько кадров прошло от начала замера

  inline void _reset_all() {
    _avr = {};
    _med = {};
    _last = {};
    _avr_frames = {};
    _med_frames = {};
    _last_frames = {};
    _start_time = {};
    _min_time = {9999};
    _max_time = {};
    _min_frames = {9999};
    _max_frames = {};
  }

  inline void _reset_point() {
    _spawn_timer = rndr(TIMER_MIN, TIMER_MAX);
    _point_pos = get_rand_pos_safe(50, 50, graphic::width, graphic::height);
    _show_point = false;
  }

  inline explicit Impl() {
     _info_text += get_locale_str("input_delay_test.info_1");
     _info_text += U" <" + hpw::keys_info.find(hpw::keycode::shoot)->name + U"> ";
     _info_text += get_locale_str("input_delay_test.info_2") + U". ";
     _info_text += get_locale_str("input_delay_test.info_3");
     _info_text += U" <" + hpw::keys_info.find(hpw::keycode::bomb)->name + U">";

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
      _last = get_cur_time() - _start_time;
      _avr.push(_last);
      _med.push(_last);
      _min_time = std::min(_min_time, _last);
      _max_time = std::max(_max_time, _last);
      
      _last_frames = _frames;
      _min_frames = std::min(_min_frames, _last_frames);
      _max_frames = std::max(_max_frames, _last_frames);
      _avr_frames.push(_last_frames);
      _med_frames.push(_last_frames);
      
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
      + U"\nAvr. " + n2s<utf32>(_avr()    * 1000., 1) + U" Ms (" + n2s<utf32>(_avr_frames()) + U" Frames)"
      + U"\nMed. " + n2s<utf32>(_med()    * 1000., 1) + U" Ms (" + n2s<utf32>(_med_frames()) + U" Frames)"
      + U"\nMin. " + n2s<utf32>(_min_time * 1000., 1) + U" Ms (" + n2s<utf32>(_min_frames)   + U" Frames)"
      + U"\nMax. " + n2s<utf32>(_max_time * 1000., 1) + U" Ms (" + n2s<utf32>(_max_frames)   + U" Frames)"
      + U"\nLast " + n2s<utf32>(_last     * 1000., 1) + U" Ms (" + n2s<utf32>(_last_frames)  + U" Frames)",
      graphic::font.get(), {5, 150, dst.X, 400}, {5, 5}
    );

    if (_show_point) {
      draw_rect_filled<&blend_diff>(dst, Rect{_point_pos, Vec{POINT_SIZE, POINT_SIZE}}, Pal8::white);
      ++_frames;
    }
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_input_delay_test)
