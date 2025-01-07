#include "util/str-util.hpp"
#include <asio/asio.hpp>
#include <atomic>
#include <cstring>
#include "udp-client.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/platform.hpp"

namespace net {
using ip_udp = asio::ip::udp;

struct Udp_client::Impl {
  Packets _packets {};
  std::atomic_bool _live {};

  inline explicit Impl(cr<Str> ip_with_port) {
    // TODO
    _live = true;
  }

  inline Impl(cr<Str> ip, u16_t port) {
    // TODO
  }

  inline ~Impl() {
    // TODO
  }

  inline bool has_packets() const { return !_packets.empty(); }
  inline cr<Packets> packets() const { return _packets; }
  inline void clear_packets() { _packets.clear(); }

  inline void try_to_connect() {
    // TODO
  }

  inline void send(cr<Bytes> bytes) {
    // TODO
  }

  inline void async_send(cr<Bytes> bytes) {
    // TODO
  }

  inline void update() {
    // TODO
  }
}; // Impl

Udp_client::Udp_client(cr<Str> ip_with_port): _impl {new_unique<Impl>(ip_with_port)} {}
Udp_client::Udp_client(cr<Str> ip, u16_t port): _impl {new_unique<Impl>(ip, port)} {}
Udp_client::~Udp_client() {}
bool Udp_client::has_packets() const { return _impl->has_packets(); }
cr<Packets> Udp_client::packets() const { return _impl->packets(); }
void Udp_client::clear_packets() { _impl->clear_packets(); }
void Udp_client::try_to_connect() { _impl->try_to_connect(); }
void Udp_client::send(cr<Bytes> bytes) { _impl->send(bytes); }
void Udp_client::async_send(cr<Bytes> bytes) { _impl->async_send(bytes); }
void Udp_client::update() { _impl->update(); }

} // net ns
