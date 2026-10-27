#include "pch.hpp"
#include "scene-empty.hpp"
#include "game/core/scenes.hpp"
#include "game/input/keys.hpp"
#include "game/core/canvas.hpp"

struct Scene_empty::Impl {
  mutable bool _screen_cleared {false};

  inline void update(const Delta_time dt) {
    // можно только выйти
    if (is_pressed_once(hpw::keycode::escape)) hpw::scene_mgr.back();
  }

  inline void draw(Image& _) const {
    // если экран не чёрный, то стереть всё с экрана
    if (_screen_cleared == false) {
      graphic::canvas->fill(Pal8::black);
      _screen_cleared = true;
    }
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_empty)
