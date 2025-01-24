#include <cassert>
#include "udp-packet-mgr.hpp"
#include "util/error.hpp"
#include "util/log.hpp"

namespace net {

struct Udp_packet_mgr::Impl {
  inline void start_server(u16_t port) {
    // TODO
  }

  inline void start_client(cr<Str> ip, u16_t port) {
    // TODO
  }

  inline void start_client(cr<Str> ip_with_port) { error("need impl"); }

  inline void update() {
    // TODO
  }

  inline void disconnect() {
    // TODO
  }

  void broadcast_push(cr<Packet> src, cr<Str> ip, const u16_t port, Udp_mgr::Action&& cb) {
    // TODO
  }
}; // Impl

Udp_packet_mgr::Udp_packet_mgr(): _impl{new_unique<Impl>()} {}
Udp_packet_mgr::~Udp_packet_mgr() {}

void Udp_packet_mgr::start_server(u16_t port) {
  Udp_mgr::start_server(port);
  _impl->start_server(port);
}

void Udp_packet_mgr::start_client(cr<Str> ip, u16_t port) {
  Udp_mgr::start_client(ip, port);
  _impl->start_client(ip, port);
}

void Udp_packet_mgr::start_client(cr<Str> ip_with_port) {
  Udp_mgr::start_client(ip_with_port);
  _impl->start_client(ip_with_port);
}

void Udp_packet_mgr::update() {
  Udp_mgr::update();
  _impl->update();
}

void Udp_packet_mgr::disconnect() {
  Udp_mgr::disconnect();
  _impl->disconnect();
}

void Udp_packet_mgr::broadcast_push(cr<Packet> src, cr<Str> ip, const u16_t port, Udp_mgr::Action&& cb)
{ _impl->broadcast_push(src, ip, port, std::move(cb)); }

} // net ns
