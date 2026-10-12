#include "pch.hpp"
#include "Scene_subpixel_test.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/util/keybits.hpp"
#include "engine/graphic/util/util-templ.hpp"

struct Scene_subpixel_test::Impl {
  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();
  }

  inline void draw(Image& dst) const {
    dst.fill({});
    
    
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_subpixel_test)
