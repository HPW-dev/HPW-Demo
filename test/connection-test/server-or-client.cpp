#include <cassert>
#include <iomanip>
#include "server-or-client.hpp"
#include "server.hpp"
#include "client.hpp"
#include "game/core/scenes.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/scene/scene-enter-text.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "graphic/image/image.hpp"
#include "util/net/net.hpp"
#include "util/unicode.hpp"

struct Server_or_client::Impl {
  Unique<Menu> _menu {};
  Str _ip_v4 {"0"};
  net::Port _port {net::DEFAULT_PORT};

  inline Impl() {
    _menu = new_unique<Text_menu>(
      Menu_items {
        new_shared<Menu_text_item>(U"сервер", [this]{ hpw::scene_mgr.add(new_shared<Server>(_ip_v4, _port)); }),
        new_shared<Menu_text_item>(U"клиент", [this]{ hpw::scene_mgr.add(new_shared<Client>(_ip_v4, _port)); }),
        new_shared<Menu_text_item>(U"задать IPv4", [this]{ hpw::scene_mgr.add(new_shared<Scene_enter_text>(
          U"Введите IPv4",
          U"Escape чтобы выйти, Enter чтобы подтвердить",
          U"IPv4: ",
          utf8_to_32(_ip_v4),
          [this](cr<utf32> text) { _ip_v4 = utf32_to_8(text); }
        )); }),
        new_shared<Menu_text_item>(U"задать порт", [this]{ hpw::scene_mgr.add(new_shared<Scene_enter_text>(
          U"Введите порт",
          U"Escape чтобы выйти, Enter чтобы подтвердить",
          U"Port: ",
          n2s<utf32>(_port),
          [this](cr<utf32> text) { _port = s2n<utf32, net::Port>(text, std::ios::dec); }
        )); }),
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
