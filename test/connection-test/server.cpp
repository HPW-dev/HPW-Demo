#include <cassert>
#include <unordered_set>
#include "server.hpp"
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

struct Server::Impl {
  constx uint BROADCAST_INTERVAL = 240 * 1.5;
  uint _broadcast_timer {BROADCAST_INTERVAL};
  uint _broadcast_count {};
  Unique<Menu> _menu {};
  net::Udp_packet_mgr _upm {};
  std::unordered_set<Str> _addressez {}; // адреса подключённых игроков
  uint _total_loaded_packets {};

  inline Impl() {
    _menu = new_unique<Text_menu>(
      Menu_items { new_shared<Menu_text_item>(get_locale_str("common.exit"), []{ hpw::scene_mgr.back(); }), },
      Vec{15, 10}
    );
    hpw::player_name = U".:Strawberry Server (Connection test):.";
    server_start();
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    _menu->update(dt);
    _upm.update();
    process_packets();

    if (--_broadcast_timer == 0) {
      _broadcast_timer = BROADCAST_INTERVAL;
      broadcast_send();
    }
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    _menu->draw(dst);
    draw_connections(dst);
  }

  inline void server_start() {
    hpw_log("start server\n");
    _upm.start_server("127.0.0.1");
    broadcast_send();
  }

  inline void broadcast_send() {
    assert(_upm.is_server());

    net::Packet broadcast_packet = new_packet<Packet_broadcast>();
    rauto raw = net::bytes_to_packet<Packet_broadcast>(broadcast_packet.bytes);
    prepare_game_version(raw.game_version);
    prepare_short_nickname(raw.short_nickname, SHORT_NICKNAME_SZ);
    raw.connected_players = _addressez.size();
    raw.hash = net::get_hash(broadcast_packet);
    hpw_log("send broadcast packet " + n2s(_broadcast_count) + " (hash: " + n2hex(raw.hash) + ")\n");

    _upm.broadcast_push(std::move(broadcast_packet));
    ++_broadcast_count;
  }

  inline void draw_connections(Image& dst) const {
    crauto font = graphic::font;
    assert(font);
    utf32 text;
    text += U"loaded packets: " + n2s<utf32>(_total_loaded_packets) + U"\n";
    text += U"connected players: " + n2s<utf32>(this->_addressez.size()) + U"\n";
    if (this->_addressez.empty()) {
      text += U"empty addrs\n";
    } else {
      for (crauto addr: _addressez) {
        // TODO player nick
        text += U"Player " + utf8_to_32(addr) + U"\n";
      }
    }
    const Vec pos(50, 60);
    font->draw(dst, pos, text);
  }

  // разбор полученных пакетов
  inline void process_packets() {
    return_if(!_upm.has_packets());
    cauto packets = _upm.unload_all();

    for (crauto packet: packets) {
      ++_total_loaded_packets;

      // пустые пакеты игнорить
      if (packet.bytes.size() < sizeof(Tag)) {
        hpw_log("packet data is small, ignore\n");
        continue;
      }

      // при несовпадении контрольной суммы игнор
      cauto local_hash = net::get_hash(packet);
      if (local_hash != net::find_packet_hash(packet)) {
        hpw_log("packet hash is not equal, ignore\n");
        continue;
      }

      cauto tag = find_packet_tag(packet);

      switch (tag) {
        case Tag::ERROR: error("tag error"); break;
        case Tag::EMPTY: hpw_log("empty tag, ignore\n"); break;
        case Tag::SERVER_BROADCAST: hpw_log("broadcast tag, ignore\n"); break;
        case Tag::CLIENT_CONNECT: process_connection(packet); break;
        default: hpw_log("unknown tag, ignore\n"); break;
      }
    } // for packets
  }

  inline void process_connection(cr<net::Packet> packet) {
    hpw_log("process connection packet...\n");

    if (packet.bytes.size() != sizeof(Packet_connect)) {
      hpw_log("размер пакета несовпадает с Packet_connect, игнор\n");
      return;
    }

    // TODO
  }
}; // Impl 

Server::Server(): _impl {new_unique<Impl>()} {}
Server::~Server() {}
void Server::update(const Delta_time dt) { _impl->update(dt); }
void Server::draw(Image& dst) const { _impl->draw(dst); }
