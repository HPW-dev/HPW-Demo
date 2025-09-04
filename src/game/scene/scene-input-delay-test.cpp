#include <cassert>
#include "scene-input-delay-test.hpp"
#include "game/core/scenes.hpp"
#include "game/core/fonts.hpp"
#include "graphic/font/font-util.hpp"
#include "game/util/locale.hpp"
#include "game/util/keybits.hpp"
#include "util/math/timer.hpp"
#include "util/math/average.hpp"
#include "util/math/random.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"

struct Scene_input_delay_test::Impl {
  utf32 _info_text {};
  Average<Delta_time, 10'000> _avr {};
  Median<Delta_time, 10'000> _med {};
  Timer _spawn_timer {};
  constx Delta_time TIMER_MIN = 3;
  constx Delta_time TIMER_MAX = 7;

  inline explicit Impl() {
     _info_text += get_locale_str("input_delay_test.info_1");
     _info_text += U" <" + hpw::keys_info.find(hpw::keycode::shoot)->name + U"> ";
     _info_text += get_locale_str("input_delay_test.info_2") + U". ";
     _info_text += get_locale_str("input_delay_test.info_3");
     _info_text += U" <" + hpw::keys_info.find(hpw::keycode::bomb)->name + U">";

     _spawn_timer = rndr(TIMER_MIN, TIMER_MAX);
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    if (is_pressed_once(hpw::keycode::bomb)) {
      _spawn_timer = rndr(TIMER_MIN, TIMER_MAX);
      _avr = {};
      _med = {};
    }

    if (is_pressed_once(hpw::keycode::shoot)) {
      _spawn_timer = rndr(TIMER_MIN, TIMER_MAX);
    }
  }

  inline void draw(Image& dst) const {
    dst.fill({});
    
    assert(graphic::font);
    text_bordered(dst,
      _info_text + U"\navr. " + n2s<utf32>(_avr()) + U"\nmed. " + n2s<utf32>(_med()),
      graphic::font.get(), {5, 150, dst.X, 400}, {5, 5}
    );
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_input_delay_test)
