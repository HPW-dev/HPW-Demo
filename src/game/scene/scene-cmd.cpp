#include <cassert>
#include <filesystem>
#include <utility>
#include <functional>
#include "scene-cmd.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/core/common.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/util/keybits.hpp"
#include "game/util/cmd.hpp"
#include "game/util/game-util.hpp"

struct Scene_cmd::Impl {

  inline Impl() {
    
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr->back();
    if (is_pressed_once(hpw::keycode::console))
      hpw::scene_mgr->back();
  }

  inline void draw(Image& dst) const {

  }
}; // impl

Scene_cmd::Scene_cmd(): impl {new_unique<Impl>()} {}
Scene_cmd::~Scene_cmd() {}
void Scene_cmd::update(const Delta_time dt) { impl->update(dt); }
void Scene_cmd::draw(Image& dst) const { impl->draw(dst); }
