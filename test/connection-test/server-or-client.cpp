#include <cassert>
#include <iomanip>
#include "server-or-client.hpp"
#include "server.hpp"
#include "client.hpp"
#include "game/core/scenes.hpp"
#include "game/core/user.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/scene/scene-enter-text.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "graphic/image/image.hpp"
#include "util/net/net.hpp"
#include "util/unicode.hpp"
#include "util/str-util.hpp"

struct Server_or_client::Impl {
  Unique<Menu> _menu {};
  Connection_ctx _ctx {};

  inline Impl() {
    _menu = new_unique<Text_menu>(
      Menu_items {
        new_shared<Menu_text_item>(U"сервер", [this]{ hpw::scene_mgr.add(new_shared<Server>(_ctx)); }),
        new_shared<Menu_text_item>(U"клиент", [this]{ hpw::scene_mgr.add(new_shared<Client>(_ctx)); }),
        new_shared<Menu_text_item>(U"задать IPv4", [this]{ hpw::scene_mgr.add(new_shared<Scene_enter_text>(
          U"Введите IPv4",
          U"Escape чтобы выйти, Enter чтобы подтвердить",
          U"IPv4: ",
          utf8_to_32(_ctx.ip_v4),
          [this](cr<utf32> text) { _ctx.ip_v4 = utf32_to_8(text); }
        )); }),
        make_port_enter_item(U"задать порт UDP сервера", _ctx.udp_server),
        make_port_enter_item(U"задать порт TCP сервера", _ctx.tcp_server),
        make_port_enter_item(U"задать порт UDP клиента", _ctx.udp_client),
        make_port_enter_item(U"задать порт TCP клиента", _ctx.tcp_client),
        new_shared<Menu_text_item>(U"задать никнейм", [this]{ hpw::scene_mgr.add(new_shared<Scene_enter_text>(
          U"Введите ник (50 символов макс.)",
          U"Escape чтобы выйти, Enter чтобы подтвердить",
          U"Никнейм: ",
          hpw::player_name,
          [this](cr<utf32> text) { hpw::player_name = text; }
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

  inline Shared<Menu_item> make_port_enter_item(cr<utf32> title, net::Port& port) {
    return new_shared<Menu_text_item> (
      title,
      [this, &port] {
        hpw::scene_mgr.add (
          new_shared<Scene_enter_text> (
            U"Введите порт",
            U"Escape чтобы выйти, Enter чтобы подтвердить",
            U"Port: ",
            n2s<utf32>(port),
            [this, &port](cr<utf32> text) { port = s2n<int>(utf32_to_8(text)); }
          ) // scene
        ); // add scene
      } // lambda
    ); // item
  }
}; // Impl 

Server_or_client::Server_or_client(): _impl {new_unique<Impl>()} {}
Server_or_client::~Server_or_client() {}
void Server_or_client::update(const Delta_time dt) { _impl->update(dt); }
void Server_or_client::draw(Image& dst) const { _impl->draw(dst); }
