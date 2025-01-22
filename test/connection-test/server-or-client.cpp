#include <cassert>
#include "server-or-client.hpp"
#include "game/core/scenes.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/util/keybits.hpp"
#include "graphic/image/image.hpp"
#include "game/util/locale.hpp"

struct Server_or_client::Impl {
  Unique<Menu> _menu {};

  inline Impl() {
    _menu = new_unique<Text_menu>(
      Menu_items {
        new_shared<Menu_text_item>(U"сервер", []{ /*TODO*/ }),
        new_shared<Menu_text_item>(U"клиент", []{ /*TODO*/ }),
        new_shared<Menu_text_item>(get_locale_str("common.exit"), []{ hpw::scene_mgr.back(); }),
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

Server_or_client::Server_or_client(): _impl {new_unique<Impl>()} {}
Server_or_client::~Server_or_client() {}
void Server_or_client::update(const Delta_time dt) { _impl->update(dt); }
void Server_or_client::draw(Image& dst) const { _impl->draw(dst); }
