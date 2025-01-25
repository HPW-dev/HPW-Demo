#include <cassert>
#include "client.hpp"
#include "test-packets.hpp"
#include "game/core/scenes.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/core/fonts.hpp"
#include "game/core/user.hpp"
#include "graphic/image/image.hpp"
#include "util/net/udp-packet-mgr.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"

struct Client::Impl {
  Unique<Menu> _menu {};
  Str _server_ipv4 {};
  utf32 _server_name {};
  decltype(Packet_broadcast::connected_players) _server_players {};
  Delta_time _server_ping {};

  inline Impl() {
    _menu = new_unique<Text_menu>(
      Menu_items {
        new_shared<Menu_text_item>(U"попытаться подключиться", [this]{ try_to_connect(); }),
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
    dst.fill(Pal8::from_real(0.171717, true));
    _menu->draw(dst);
    draw_server_info(dst);
  }

  inline void try_to_connect() {
    if (_server_ipv4.empty()) {
      hpw_log("нету адреса для подключения\n");
      return;
    }

    hpw_log("попытка подключения к серверу: \"" + _server_ipv4 + "\"\n");
    // TODO
  }

  inline void draw_server_info(Image& dst) const {
    crauto font = graphic::font;
    assert(font);
    utf32 text = U"S E R V E R\n";
    text += U"* IPv4 " + (_server_ipv4.empty() ? U"-" : utf8_to_32(_server_ipv4)) + U"\n";
    text += U"* Name " + (_server_name.empty() ? U"-" : _server_name) + U"\n";
    text += U"* Players " + (_server_ipv4.empty() ? U"-" : n2s<utf32>(_server_players)) + U"\n";
    text += U"* Ping " + (_server_ipv4.empty() ? U"-" : n2s<utf32>(_server_ping)) + U" ms.\n";
    const Vec pos(50, 60);
    font->draw(dst, pos, text);
  }
}; // Impl 

Client::Client(): _impl {new_unique<Impl>()} {}
Client::~Client() {}
void Client::update(const Delta_time dt) { _impl->update(dt); }
void Client::draw(Image& dst) const { _impl->draw(dst); }
