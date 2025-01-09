#include <utility>
#include "scene-netplay-menu.hpp"
#include "game/core/canvas.hpp"
#include "game/core/scenes.hpp"
#include "game/util/locale.hpp"
#include "game/util/keybits.hpp"
#include "game/menu/item/text-item.hpp"
#include "graphic/image/image.hpp"

struct Scene_netplay_menu::Impl {
  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();
    //menu.update(dt);
  }

  inline void draw(Image& dst) const {
    //menu.draw(dst);
  }
}; // Impl 

Scene_netplay_menu::Scene_netplay_menu(): _impl {new_unique<Impl>()} {}
Scene_netplay_menu::~Scene_netplay_menu() {}
void Scene_netplay_menu::update(const Delta_time dt) { _impl->update(dt); }
void Scene_netplay_menu::draw(Image& dst) const { _impl->draw(dst); }
