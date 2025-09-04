#include <cassert>
#include "scene-connect-by-ipv4.hpp"
#include "game/menu/menu-from-yaml.hpp"
#include "game/menu/item/item.hpp"
#include "game/core/scenes.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/keybits.hpp"
#include "util/file/file.hpp"
#include "util/file/yaml.hpp"
#include "graphic/image/image.hpp"

struct Scene_connect_by_ipv4::Impl {
  Unique<Menu> _menu {};

  inline Impl() {
    cauto config_file = load_res("resource/menu/connect by ipv4.yml");
    Yaml config(config_file);
    _menu = menu_from_yaml(
      config,
      Action_table {
        /*{"goto_find_server_scene", Action_container( Action([]{
          hpw::scene_mgr.add(new_shared<Scene_connect_by_ipv4>());
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

Scene_connect_by_ipv4::Scene_connect_by_ipv4(): _impl {new_unique<Impl>()} {}
Scene_connect_by_ipv4::~Scene_connect_by_ipv4() {}
void Scene_connect_by_ipv4::update(const Delta_time dt) { _impl->update(dt); }
void Scene_connect_by_ipv4::draw(Image& dst) const { _impl->draw(dst); }
