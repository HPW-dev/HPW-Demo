#include <cassert>
#include "scene-create-server.hpp"
#include "game/menu/menu-from-yaml.hpp"
#include "game/menu/item/item.hpp"
#include "game/core/scenes.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/keybits.hpp"
#include "util/file/file.hpp"
#include "util/file/yaml.hpp"
#include "graphic/image/image.hpp"

struct Scene_create_server::Impl {
  Unique<Menu> _menu {};

  inline Impl() {
    cauto config_file = load_res("resource/menu/create server.yml");
    Yaml config(config_file);
    _menu = menu_from_yaml(
      config,
      Action_table {
        /*{"goto_find_server_scene", Action_container( Menu_item::Action([]{
          hpw::scene_mgr.add(new_shared<Scene_create_server>());
        }) )},*/
      }
    );
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

Scene_create_server::Scene_create_server(): _impl {new_unique<Impl>()} {}
Scene_create_server::~Scene_create_server() {}
void Scene_create_server::update(const Delta_time dt) { _impl->update(dt); }
void Scene_create_server::draw(Image& dst) const { _impl->draw(dst); }
