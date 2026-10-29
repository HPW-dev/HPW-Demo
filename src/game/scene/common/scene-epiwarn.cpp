#include "pch.hpp"
#include "scene-epiwarn.hpp"

#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "game/input/keys.hpp"


struct Scene_epiwarn::Impl {
  void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape)) hpw::scene_mgr.back();
  }

  void draw(Image& dst) const {

  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_epiwarn)
