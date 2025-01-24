#include <cassert>
#include <utility>

// порядок инклудов не нарушать, иначе взрыв!
#include "util/str-util.hpp"
#include "udp-packet-mgr.hpp"
#include <asio/asio.hpp>

#include "util/error.hpp"
#include "util/log.hpp"

namespace net {
using ip_udp = asio::ip::udp;

struct Udp_packet_mgr::Impl {
  struct Status {
    bool is_server: 1 {};
    bool is_active: 1 {};
  };

  Status _status {};
  mutable asio::io_service _io {};
  Unique<ip_udp::socket> _socket {};
  Port _port {net::DEFAULT_PORT}; // свой порт
  Str _ip_v4 {net::MY_IPV4}; // свой IPv4

  inline ~Impl() { disconnect(); }

  inline void start_server(Port port) {
    // TODO
  }

  inline void start_client(cr<Str> ip, Port port) {
    // TODO
  }

  inline void update() {
    // TODO
  }

  inline void disconnect() {
    // TODO
  }

  inline void broadcast_push(Packet&& src, const Port port, Action&& cb) {
    // TODO
  }

  inline cr<Port> port() const { return _port; }
  inline cr<Str> ip_v4() const { return _ip_v4; }
  inline bool is_server() const { return _status.is_active && _status.is_server; }
  inline bool is_client() const { return _status.is_active && !_status.is_server; }
}; // Impl

Udp_packet_mgr::Udp_packet_mgr(): _impl{new_unique<Impl>()} {}
Udp_packet_mgr::~Udp_packet_mgr() {}
void Udp_packet_mgr::start_server(Port port) { _impl->start_server(port); }
void Udp_packet_mgr::start_client(cr<Str> ip, Port port) { _impl->start_client(ip, port); }
void Udp_packet_mgr::update() { _impl->update(); }
void Udp_packet_mgr::broadcast_push(Packet&& src, const Port port, Action&& cb)
  { _impl->broadcast_push(std::move(src), port, std::move(cb)); }
cr<Port> Udp_packet_mgr::port() const { return _impl->port(); }
cr<Str> Udp_packet_mgr::ip_v4() const { return _impl->ip_v4(); }
bool Udp_packet_mgr::is_server() const { return _impl->is_server(); }
bool Udp_packet_mgr::is_client() const { return _impl->is_client(); }

} // net ns
