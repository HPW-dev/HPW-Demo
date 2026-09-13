#include "pch.hpp"
#include "scene-dummy.hpp"
#include "engine/graphic/util/graphic-util.hpp"
#include "game/core/scenes.hpp"
#include "game/util/keybits.hpp"

struct Scene_dummy::Impl {
  inline explicit Impl() {}

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();
    
    // TODO
  }

  inline void draw(Image& dst) const {
    // TODO
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_dummy)
