#include <cassert>
#include "scene-input-delay-test.hpp"
#include "game/core/scenes.hpp"
#include "game/util/locale.hpp"
#include "game/util/keybits.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"

struct Scene_input_delay_test::Impl {
  inline explicit Impl() {}

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();
  }

  inline void draw(Image& dst) const {
    dst.fill({});
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_input_delay_test)
