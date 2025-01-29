#include <cassert>
#include <utility>
#include <deque>

// порядок инклудов не нарушать, иначе взрыв!
#include "util/str-util.hpp"
#include "net-packet-mgr.hpp"
#include <asio/asio.hpp>

#include "util/error.hpp"
#include "util/log.hpp"

namespace net {
using ip_udp = asio::ip::udp;
using ip_tcp = asio::ip::tcp;

struct Packet_mgr::Impl {
  Packet_mgr::Status _status {};

  inline Impl() {
    // TODO
  }

  inline void start(cr<Config> cfg) {
    // TODO
  }

  inline void disconnect() {
    // TODO
  }

  inline void update() {
    // TODO
  }

  inline void send(cr<Packet> src, cr<Target_info> target) {
    // TODO
  }

  inline Packets unload_all() {
    return {}; // TODO
  }

  inline cr<Status> status() const { return _status; }
  inline ~Impl() { disconnect(); }
}; // Impl

Packet_mgr::Packet_mgr(): _impl{new_unique<Impl>()} {}
Packet_mgr::~Packet_mgr() {}
void Packet_mgr::start(cr<Packet_mgr::Config> cfg) { _impl->start(cfg); }
void Packet_mgr::disconnect() { _impl->disconnect(); }
void Packet_mgr::update() { _impl->update(); }
void Packet_mgr::send(cr<Packet> src, cr<Target_info> target) { _impl->send(src, target); }
Packets Packet_mgr::unload_all() { return _impl->unload_all(); }
cr<Packet_mgr::Status> Packet_mgr::status() const { return _impl->status(); }

} // net ns
