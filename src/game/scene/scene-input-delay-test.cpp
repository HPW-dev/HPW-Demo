#include <cassert>
#include "scene-input-delay-test.hpp"
#include "game/core/scenes.hpp"
#include "game/core/fonts.hpp"
#include "graphic/font/font-util.hpp"
#include "game/util/locale.hpp"
#include "game/util/keybits.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"

struct Scene_input_delay_test::Impl {
  utf32 info_text {};

  inline explicit Impl() {
     info_text += get_locale_str("input_delay_test.info_1");
     info_text += U" <" + hpw::keys_info.find(hpw::keycode::shoot)->name + U"> ";
     info_text += get_locale_str("input_delay_test.info_2") + U". ";
     info_text += get_locale_str("input_delay_test.info_3");
     info_text += U" <" + hpw::keys_info.find(hpw::keycode::bomb)->name + U">";
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    if (is_pressed_once(hpw::keycode::bomb))
      {}

    if (is_pressed_once(hpw::keycode::shoot))
      {}
  }

  inline void draw(Image& dst) const {
    dst.fill({});
    text_bordered(dst, info_text, graphic::font.get(), {5, 150, dst.X, 400}, {5, 5});
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_input_delay_test)
