#include <cassert>
#include <unordered_map>
#include "server.hpp"
#include "test-packets.hpp"
#include "game/scene/msgbox/msgbox-enter.hpp"
#include "game/core/scenes.hpp"
#include "game/core/user.hpp"
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

struct Player_info {
  Str ip_v4 {};
  utf32 short_nickname {};
};

struct Server::Impl {
  constx uint BROADCAST_INTERVAL = 240 * 1.5;
  uint _broadcast_timer {BROADCAST_INTERVAL};
  uint _broadcast_count {};
  Unique<Menu> _menu {};
  net::Udp_packet_mgr _upm {};
  std::unordered_map<Str, Player_info> _players {}; // адреса подключённых игроков
  uint _total_loaded_packets {};

  inline explicit Impl(cr<Str> ip_v4, const net::Port port) {
    _menu = new_unique<Text_menu>(
      Menu_items { new_shared<Menu_text_item>(get_locale_str("common.exit"), []{ hpw::scene_mgr.back(); }), },
      Vec{15, 10}
    );

    if (hpw::player_name.empty())
      hpw::player_name = U".:Strawberry Server (Connection test):.";

    server_start(ip_v4, port);
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    _menu->update(dt);

    if (_upm.is_active()) {
      _upm.update();
      process_packets();

      if (--_broadcast_timer == 0) {
        _broadcast_timer = BROADCAST_INTERVAL;
        broadcast_send();

        // дать подключившимся по ipv4 игрокам инфу о сервере
        for (crauto [ipv4, info]: _players) {
          _upm.push(get_server_info_packet(), info.ip_v4);
        }
      } // if broadcast timer
    } // if _ump active
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    _menu->draw(dst);
    draw_connections(dst);
  }

  inline void server_start(cr<Str> ip_v4, const net::Port port) {
    hpw_log("start server\n");
    try {
      _upm.start_server(ip_v4, port);
      broadcast_send();
    } catch (cr<hpw::Error> err) {
      hpw::scene_mgr.add( new_shared<Scene_msgbox_enter>(U"ошибка при создании сервера: " +
        utf8_to_32(err.what()), get_locale_str("common.warning")) );
      hpw::scene_mgr.back();
    } catch (...) {
      hpw::scene_mgr.add( new_shared<Scene_msgbox_enter>(U"неизвестная ошибка при создании сервера",
        get_locale_str("common.warning")) );
      hpw::scene_mgr.back();
    }
  }

  inline void broadcast_send() {
    assert(_upm.is_server());
    _upm.broadcast_push(get_server_info_packet());
    ++_broadcast_count;
  }

  inline void draw_connections(Image& dst) const {
    crauto font = graphic::font;
    assert(font);
    utf32 text;
    text += U"loaded packets: " + n2s<utf32>(_total_loaded_packets) + U"\n";
    text += U"connected players: " + n2s<utf32>(this->_players.size()) + U"\n";
    if (this->_players.empty()) {
      text += U"empty player list\n";
    } else {
      for (crauto player: _players) {
        text += U"Player " + utf8_to_32(player.second.ip_v4) +
          U" \"" + player.second.short_nickname + U"\"\n";
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

    crauto raw = net::bytes_to_packet<Packet_connect>(packet.bytes);
    
    // version check:
    Version local_ver;
    prepare_game_version(local_ver);
    if (local_ver != raw.game_version)
      hpw_log("версии игры не совпадают\n");

    // save player info
    _players[packet.ip_v4] = Player_info {
      .ip_v4 = packet.ip_v4,
      .short_nickname = raw.short_nickname
    };

    // сразу дать игроку знать что сейчас на сервере
    _upm.push(get_server_info_packet(), packet.ip_v4);
  }

  inline net::Packet get_server_info_packet() const {
    net::Packet ret = new_packet<Packet_server_info>();
    rauto raw = net::bytes_to_packet<Packet_server_info>(ret.bytes);
    prepare_game_version(raw.game_version);
    prepare_short_nickname(raw.short_nickname, SHORT_NICKNAME_SZ);
    raw.connected_players = _players.size();
    raw.hash = net::get_hash(ret);
    return ret;
  }
}; // Impl 

Server::Server(cr<Str> ip_v4, const net::Port port): _impl {new_unique<Impl>(ip_v4, port)} {}
Server::~Server() {}
void Server::update(const Delta_time dt) { _impl->update(dt); }
void Server::draw(Image& dst) const { _impl->draw(dst); }
