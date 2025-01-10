#include <cassert>
#include "scene-netplay-menu.hpp"
#include "game/menu/menu-from-yaml.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/core/scenes.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/keybits.hpp"
#include "util/file/file.hpp"
#include "util/file/yaml.hpp"
#include "util/error.hpp" // TODO del
#include "graphic/image/image.hpp"

struct Scene_netplay_menu::Impl {
  Menu_text_item::Action _goto_find_server_scene {};
  Menu_text_item::Action _goto_connect_by_ipv4_scene {};
  Menu_text_item::Action _goto_create_server_scene {};
  Action_table _actions {};
  Unique<Menu> _menu {};

  inline Impl() {
    _goto_find_server_scene = []{ error("need impl"); };
    _goto_connect_by_ipv4_scene = []{ error("need impl"); };
    _goto_create_server_scene = []{ error("need impl"); };
    _actions = Action_table {
      {"goto_find_server_scene", Action_container(_goto_find_server_scene)},
      {"goto_connect_by_ipv4_scene", Action_container(_goto_connect_by_ipv4_scene)},
      {"goto_create_server_scene", Action_container(_goto_create_server_scene)},
    };

    cauto config_file = load_res("resource/menu/netplay-menu.yml");
    Yaml config(config_file);
    _menu = menu_from_yaml(config, _actions);
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

Scene_netplay_menu::Scene_netplay_menu(): _impl {new_unique<Impl>()} {}
Scene_netplay_menu::~Scene_netplay_menu() {}
void Scene_netplay_menu::update(const Delta_time dt) { _impl->update(dt); }
void Scene_netplay_menu::draw(Image& dst) const { _impl->draw(dst); }
