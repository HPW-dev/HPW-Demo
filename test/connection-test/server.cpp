#include <cassert>
#include "server.hpp"
#include "game/core/scenes.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/core/fonts.hpp"
#include "graphic/image/image.hpp"
#include "util/net/udp-packet-mgr.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"

struct Server::Impl {
  constx uint BROADCAST_INTERVAL = 240*1;
  uint _broadcast_timer {BROADCAST_INTERVAL};
  uint _broadcast_count {};
  Unique<Menu> _menu {};
  net::Udp_packet_mgr _upm {};

  inline Impl() {
    _menu = new_unique<Text_menu>(
      Menu_items {
        new_shared<Menu_text_item>(get_locale_str("common.exit"), []{ hpw::scene_mgr.back(); }),
      },
      Vec{15, 10}
    );

    server_start();
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    _menu->update(dt);

    if (--_broadcast_timer == 0) {
      _broadcast_timer = BROADCAST_INTERVAL;
      broadcast_send();
    }
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    _menu->draw(dst);
  }

  inline void server_start() {
    hpw_log("start server\n");
    _upm.start_server(net::DEFAULT_PORT);
    broadcast_send();
  }

  inline void broadcast_send() {
    assert(_upm.is_server());
    hpw_log("send broadcast packet " + n2s(_broadcast_count) + "\n");
    // TODO
    ++_broadcast_count;
  }
}; // Impl 

Server::Server(): _impl {new_unique<Impl>()} {}
Server::~Server() {}
void Server::update(const Delta_time dt) { _impl->update(dt); }
void Server::draw(Image& dst) const { _impl->draw(dst); }
