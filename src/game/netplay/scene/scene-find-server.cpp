#include "pch.hpp"
#include "scene-find-server.hpp"
#include "game/menu/menu-from-json.hpp"
#include "game/menu/item/item.hpp"
#include "game/core/scenes.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/keybits.hpp"
#include "util/file/file.hpp"
//#include "util/net/tcp-mgr.hpp"
#include "engine/graphic/image/image.hpp"

struct Scene_find_server::Impl {
  Unique<Menu> _menu {};

  inline Impl() {
    _menu = menu_from_json(json_from_res("scripts/ui/menu/find server.json"), Action_table {
        /*{"goto_find_server_scene", Action_container( Action([]{
          hpw::scene_mgr.add(new_shared<Scene_find_server>());
        }) )},*/
    } );
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    assert(_menu);
    _menu->update(dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);

    assert(_menu);
    _menu->draw(dst);
  }
}; // Impl 

Scene_find_server::Scene_find_server(): _impl {new_unique<Impl>()} {}
Scene_find_server::~Scene_find_server() {}
void Scene_find_server::update(const Delta_time dt) { _impl->update(dt); }
void Scene_find_server::draw(Image& dst) const { _impl->draw(dst); }
