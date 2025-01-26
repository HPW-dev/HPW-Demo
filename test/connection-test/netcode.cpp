#include <cassert>
#include <unordered_map>
#include <sstream>
#include "netcode.hpp"
#include "test-packets.hpp"
#include "game/core/scenes.hpp"
#include "game/core/fonts.hpp"
#include "game/core/user.hpp"
#include "game/util/keybits.hpp"
#include "graphic/image/image.hpp"
#include "util/net/udp-packet-mgr.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/math/random.hpp"

struct Player_info {
  utf32 nickname {};
  bool connected {};
};

struct Netcode::Impl {
  net::Udp_packet_mgr _upm {};
  std::unordered_map<Str, Player_info> _players {}; // <Ip, Info>

  inline explicit Impl(bool is_server, cr<Str> ip_v4, const net::Port port) {
    hpw_log (
      "start " + Str(is_server ? "server" : "client") + " mode:\n" +
      "- input ip: " + ip_v4 + "\n" +
      "- input port: " + n2s(port) + "\n"
    );

    if (is_server)
      _upm.start_server(ip_v4, port);
    else
      _upm.start_client(ip_v4, port);

    // если кастомного имени, сгенерить случайное
    if (hpw::player_name.empty()) {
      hpw::player_name = U"ʕ•ᴥ•ʔ Тестовый игрок №";
      cauto rnd_num = rndu_fast(999);
      hpw::player_name += n2s<utf32>(rnd_num);
    }

    // добавить самого себя в список игроков
    _players[_upm.ip_v4()] = Player_info {
      .nickname = hpw::player_name,
      .connected = true
    };
  }

  inline void draw(Image& dst) const {
    crauto font = graphic::font;
    assert(font);
    std::stringstream ss;
    ss << "N E T P L A Y   I N F O :\n";
    ss << "- self name: " << utf32_to_8(hpw::player_name) << "\n";
    ss << "- players:";
    if (_players.empty()) {
      ss << "empty\n";
    } else {
      ss << "\n";
      for (crauto [ip, info]: _players) {
        ss << "  * " << ip << ":" << _upm.port() << " ";
        ss << utf32_to_8(info.nickname) << " - ";
        ss << (info.connected ? "connected" : "waiting for connection...") << "\n";
      }
    }
    const Vec pos(15, 90);
    font->draw(dst, pos, utf8_to_32(ss.str()));
  }

  inline void update(const Delta_time dt) {
    // TODO
  }

  inline void connect_to_broadcast() {
    // TODO
  }

  inline void connect_to(cr<Str> ip_v4) {
    // TODO
  }
};

Netcode::Netcode(bool is_server, cr<Str> ip_v4, const net::Port port)
  : _impl{new_unique<Impl>(is_server, ip_v4, port)} {}
Netcode::~Netcode() {}
void Netcode::connect_to(cr<Str> ip_v4) { _impl->connect_to(ip_v4); }
void Netcode::connect_to_broadcast() { _impl->connect_to_broadcast(); }
void Netcode::draw(Image& dst) const { _impl->draw(dst); }
void Netcode::update(const Delta_time dt) { _impl->update(dt); }
