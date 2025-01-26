#include "client.hpp"
#include "netcode.hpp"
#include "game/core/scenes.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/scene/scene-enter-text.hpp"
#include "graphic/image/image.hpp"
#include "util/str-util.hpp"

struct Client::Impl {
  Unique<Netcode> _netcode {};
  Unique<Menu> _menu {};
  Str _server_ipv4 {};

  inline explicit Impl(cr<Str> ip_v4, const net::Port port)
  : _netcode {new_unique<Netcode>(false, ip_v4, port)}
  {
    _menu = new_unique<Text_menu>(
      Menu_items {
        new_shared<Menu_text_item>(U"попытаться подключиться", [this]{ _netcode->connect_to_broadcast(); }),

        new_shared<Menu_text_item>(U"подключиться по IPv4", [this]{
          hpw::scene_mgr.add(new_shared<Scene_enter_text>(
            U"Введите IP сервера",
            U"Enter для подтверждения, Escape чтобы выйти",
            U"Server IPv4: ",
            utf8_to_32(_server_ipv4),
            [this](cr<utf32> text) {            
              _server_ipv4 = utf32_to_8(text);
              _netcode->connect_to(_server_ipv4);
            }
          ));
        }), // кнопка подключиться по ipv4

        new_shared<Menu_text_item>(get_locale_str("common.exit"), []{ hpw::scene_mgr.back(); }),
      },
      Vec{15, 10}
    );
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    _menu->update(dt);
    _netcode->update(dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::from_real(0.171717, true));
    _menu->draw(dst);
    _netcode->draw(dst);
  }
}; // Impl 

Client::Client(cr<Str> ip_v4, const net::Port port): _impl {new_unique<Impl>(ip_v4, port)} {}
Client::~Client() {}
void Client::update(const Delta_time dt) { _impl->update(dt); }
void Client::draw(Image& dst) const { _impl->draw(dst); }
