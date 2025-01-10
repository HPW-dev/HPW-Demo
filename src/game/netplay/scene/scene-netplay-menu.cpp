#include <utility>
#include "scene-netplay-menu.hpp"
#include "game/core/canvas.hpp"
#include "game/core/scenes.hpp"
#include "game/util/locale.hpp"
#include "game/util/keybits.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/menu-from-yaml.hpp"
#include "graphic/image/image.hpp"
#include "util/log.hpp" // TODO del

struct Scene_netplay_menu::Impl {
  Unique<Text_menu> _menu {};
  Unique<Menu> _menu2 {};

  inline Impl() {
    // _menu2 = menu_from_yaml(); TODO

    init_unique (
      _menu,
      Menu_items {
        new_shared<Menu_text_item> (
          get_locale_str("netplay.find_server"),
          []{ hpw_info("need impl\n"); },
          []->utf32 { return {}; },
          get_locale_str("netplay.find_server_desc")
        ),
        new_shared<Menu_text_item> (
          get_locale_str("netplay.join_by_ipv4port"),
          []{ hpw_info("need impl\n"); },
          []->utf32 { return {}; },
          get_locale_str("netplay.join_by_ipv4port_desc")
        ),
        new_shared<Menu_text_item> (
          get_locale_str("netplay.start_server"),
          []{ hpw_info("need impl\n"); },
          []->utf32 { return {}; },
          get_locale_str("netplay.start_server_desc")
        ),
        new_shared<Menu_text_item>(get_locale_str("common.back"), []{ hpw::scene_mgr.back(); }),
      },
      Vec{15, 10}
    );
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    _menu->update(dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    _menu->draw(dst);
  }
}; // Impl 

Scene_netplay_menu::Scene_netplay_menu(): _impl {new_unique<Impl>()} {}
Scene_netplay_menu::~Scene_netplay_menu() {}
void Scene_netplay_menu::update(const Delta_time dt) { _impl->update(dt); }
void Scene_netplay_menu::draw(Image& dst) const { _impl->draw(dst); }
