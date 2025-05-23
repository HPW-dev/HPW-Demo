#include <cassert>
#include <unordered_map>
#include <sstream>
#include "netcode.hpp"
#include "packet/connection-info.hpp"
#include "packet/connected.hpp"
#include "packet/disconnected.hpp"
#include "game/core/scenes.hpp"
#include "game/core/fonts.hpp"
#include "game/core/user.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/scene/msgbox/msgbox-enter.hpp"
#include "graphic/image/image.hpp"
#include "util/net/net-packet-mgr.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/math/random.hpp"

struct Netcode::Impl {
  constx uint BACKGROUND_ACTIONS_TIMER = 240 * 1.5;
  uint _background_actions_timer {BACKGROUND_ACTIONS_TIMER};
  net::Packet_mgr _pck_mgr {};
  std::unordered_map<Str, net::Player_info> _players {}; // <Ip, Info>
  uint _ignored_packets {};
  Str _server_ip {};
  net::Port _target_tcp_port {}; // к какому TCP порту подключаться
  net::Port _target_udp_port {}; // к какому UDP порту подключаться

  inline explicit Impl(bool is_server, cr<Connection_ctx> ctx) {
    try {
      if (is_server) {
        _pck_mgr.start_server(ctx.ip_v4, ctx.udp_server, ctx.tcp_server);
        _target_udp_port = ctx.udp_client;
        _target_tcp_port = ctx.tcp_client;
      } else {
        _pck_mgr.start_client(ctx.ip_v4, ctx.udp_client, ctx.tcp_client);
        _target_udp_port = ctx.udp_server;
        _target_tcp_port = ctx.tcp_server;
      }
    } catch (...) {
      hpw::scene_mgr.add( new_shared<Scene_msgbox_enter>(
        U"ошибка при установлении соединения, проверьте настройки\n",
        get_locale_str("common.warning")
      ) );
      hpw::scene_mgr.back();
      return;
    }

    log_info << ("created " + Str(_pck_mgr.is_server() ? "server" : "client") +
      " at ip:port udp/tcp " + _pck_mgr.ip_v4() + ":" +
      n2s(_pck_mgr.udp_port()) + "/" +  n2s(_pck_mgr.tcp_port()) + "\n");

    // если кастомного имени, сгенерить случайное
    if (hpw::player_name.empty()) {
      hpw::player_name = U"ʕ•ᴥ•ʔ Игрок №";
      cauto rnd_num = rndu_fast(999);
      hpw::player_name += n2s<utf32>(rnd_num);
    }
  }

  inline ~Impl() {
    if (_pck_mgr.is_server())
      _pck_mgr.broadcast_push(net::Pck_disconnected().to_packet(), _target_udp_port);
    elif (!_server_ip.empty())
      _pck_mgr.push(net::Pck_disconnected().to_packet(), _server_ip, _target_tcp_port, false);
  }

  inline void draw(Image& dst) const {
    crauto font = graphic::system_mono;
    assert(font);

    std::stringstream ss;
    ss << "N E T P L A Y   I N F O :\n";
    ss << "- self name \"" << utf32_to_8(hpw::player_name) << "\"\n";
    ss << "- data packets:\n";
    ss << "  * sended " << _pck_mgr.sended_packets() << "\n";
    ss << "  * received " << _pck_mgr.received_packets() << "\n";
    ss << "  * ignored " << _ignored_packets << "\n";
    ss << "- players:";

    // показать игроков
    if (_players.empty()) {
      ss << "empty\n";
    } else {
      ss << "\n";
      for (crauto [ip, info]: _players) {
        ss << "  * " << ip << ":" << _pck_mgr.tcp_port() << " ";
        ss << "\"" << utf32_to_8(info.nickname) << "\" - ";
        ss << (info.connected ? "connected" : "waiting for connection...") << "\n";
      }
    }

    const Vec pos(15, 90);
    font->draw(dst, pos, utf8_to_32(ss.str()));
  }

  inline void update(const Delta_time dt) {
    _pck_mgr.update();

    if (--_background_actions_timer == 0) {
      _background_actions_timer = BACKGROUND_ACTIONS_TIMER;

      if (_pck_mgr.is_server()) {
        _pck_mgr.broadcast_push(get_broadcast_packet(), _target_udp_port);

        // дать игрокам знать что они законнектились
        for (crauto [addr, player]: _players) {
          _pck_mgr.push(net::Pck_connected().to_packet(), addr, _target_tcp_port, false);
        }
      } else { // client
        // дать серваку знать о успешном подключении
        if (!_server_ip.empty()) {
          _pck_mgr.push(net::Pck_connected().to_packet(), _server_ip, _target_tcp_port, false);
        }
      }
    } // background actions timer

    process_packets();
  }

  inline void connect_to_broadcast() {
    error("need impl");
  }

  inline void connect_to(cr<Str> ip_v4) {
    try {
      _pck_mgr.send(get_connect_packet(), ip_v4, _target_tcp_port, false);
    } catch (...) {
      hpw::scene_mgr.add( new_shared<Scene_msgbox_enter>(
        U"не удалось подключиться по IP: " + utf8_to_32(ip_v4) + U"\n",
        get_locale_str("common.warning")
      ) );
    }
  }

  inline net::Packet get_broadcast_packet() {
    net::Pck_connection_info raw;
    raw.is_server = _pck_mgr.is_server();
    raw.self_nickname = hpw::player_name;
    for (crauto [_, player]: _players)
      raw.players.push_back(player);
    return raw.to_packet();
  }

  inline void process_packets() {
    return_if(!_pck_mgr.is_active());
    return_if(!_pck_mgr.has_packets());

    for (crauto packet: _pck_mgr.unload_all()) {
      try {
        switch (net::get_packet_tag(packet)) {
          case net::Tag::EMPTY: {
            hpw_warning("пакет с пустой меткой. Источник " + packet.ip_v4 + "\n");
            ++_ignored_packets;
            break;
          }
          case net::Tag::MESSAGE: process_message(packet); break;
          case net::Tag::CONNECTION_INFO: process_connection_info(packet); break;
          case net::Tag::DISCONNECTED: process_disconnect(packet); break;
          case net::Tag::CONNECTED: process_connected(packet); break;
          default: {
            hpw_warning("неизвестная метка пакета. Игнор пакета...\n");
            ++_ignored_packets;
            break;
          }
        }
      } catch (cr<hpw::Error> err) {
        hpw_warning(Str("Ошибка при обработке пакета: ") + err.what() + ". Игнор пакета...\n");
        ++_ignored_packets;
      } catch (...) {
        hpw_warning("Неизвестная ошибка при обработке пакета. Игнор пакета...\n");
        ++_ignored_packets;
      }
    } // for packets
  }

  inline void process_message(cr<net::Packet> src) {
    error("need impl");
  }

  inline void process_connection_info(cr<net::Packet> src) {
    net::Pck_connection_info raw;
    raw.from_packet(src);

    if (_pck_mgr.is_server()) {
      if (raw.is_server) {
        log_debug("игнор пакета с инфой о подключении от " + src.ip_v4 + "\n");
        ++_ignored_packets;
      } else {
        log_debug("пытается покдлючиться игрок " + utf32_to_8(raw.self_nickname)
          + "   ip: " + src.ip_v4 + "\n");
        // добавить игрока в список
        _players[src.ip_v4] = net::Player_info {
          .nickname = raw.self_nickname,
          .ip_v4 = src.ip_v4,
          .connected = false,
        };
      }
    } else { // client
      for (crauto player: raw.players)
        _players[player.ip_v4] = net::Player_info {
          .nickname = player.nickname,
          .ip_v4 = player.ip_v4,
          .connected = player.connected,
        };
      _players[src.ip_v4] = net::Player_info {
        .nickname = raw.self_nickname,
        .ip_v4 = src.ip_v4,
        .connected = true,
      };
      _server_ip = src.ip_v4;

      std::stringstream info;
      info << "получен пакет с инфой о подключении:\n";
      info << "- имя сервера: " << utf32_to_8(raw.self_nickname) << "\n";
      info << "- число игроков на сервере: " << raw.players.size() << "\n";
      hpw_debug(info.str());
    }
  }

  inline void process_disconnect(cr<net::Packet> src) {
    net::Pck_disconnected raw;
    raw.from_packet(src);
    iferror (raw.disconnect_you, "need impl");
    
    if (_pck_mgr.is_server()) {
      try {
        _players.at(src.ip_v4).connected = false;
      } catch (...) {}
    } else {
      _server_ip = {};
      _players.clear();
    }
  }

  inline void process_connected(cr<net::Packet> src) {
    net::Pck_connected raw;
    raw.from_packet(src);
    
    try {
      _players.at(src.ip_v4).connected = true;
    } catch (...) {}
  }

  inline net::Packet get_connect_packet() {
    net::Pck_connection_info raw;
    raw.is_server = false;
    raw.self_nickname = hpw::player_name;
    return raw.to_packet();
  }
};

Netcode::Netcode(bool is_server, cr<Connection_ctx> ctx): _impl{new_unique<Impl>(is_server, ctx)} {}
Netcode::~Netcode() {}
void Netcode::connect_to(cr<Str> ip_v4) { _impl->connect_to(ip_v4); }
void Netcode::connect_to_broadcast() { _impl->connect_to_broadcast(); }
void Netcode::draw(Image& dst) const { _impl->draw(dst); }
void Netcode::update(const Delta_time dt) { _impl->update(dt); }
