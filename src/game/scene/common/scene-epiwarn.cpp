#include "pch.hpp"
#include "scene-epiwarn.hpp"

#include "engine/graphic/util/graphic-util.hpp"
#include "game/util/sprite-helper.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/input/keys.hpp"


struct Scene_epiwarn::Impl {
  cr<Image> _warn_scr;

  Impl()
  : _warn_scr{find_image_safe("graphic/images/misc/epilepsy-warning.png")}
  {}

  void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape)) hpw::scene_mgr.back();
  }

  void draw(Image& dst) const {

  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_epiwarn)
