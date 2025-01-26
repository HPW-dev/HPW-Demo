#include <cassert>
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

struct Netcode::Impl {
  net::Udp_packet_mgr _upm {};

  inline explicit Impl(bool is_server, cr<Str> ip_v4, const net::Port port)
  {
    hpw_log (
      "start " + Str(is_server ? "server" : "client") + " mode:\n" +
      "- input ip: " + ip_v4 + "\n" +
      "- input port: " + n2s(port) + "\n"
    );
    if (is_server)
      _upm.start_server(ip_v4, port);
    else
      _upm.start_client(ip_v4, port);
  }

  inline void draw(Image& dst) const {
    // TODO
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
