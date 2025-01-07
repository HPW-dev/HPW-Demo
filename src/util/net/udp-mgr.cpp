#include <utility>
#include "udp-mgr.hpp"
#include "util/error.hpp"

namespace net {

struct Udp_mgr::Impl {
  inline bool is_server() const {
    // TODO
    error("need impl");
    return {};
  }

  inline bool is_client() const {
    // TODO
    error("need impl");
    return {};
  }

  inline operator bool() const {
    // TODO
    error("need impl");
    return {};
  }

  inline Strs avaliable_ipv4s() const {
    // TODO
    error("need impl");
    return {};
  }

  inline void start_server(u16_t port) {
    // TODO
    error("need impl");
  }

  inline void start_client(cr<Str> ip, u16_t port) {
    // TODO
    error("need impl");
  }

  inline void start_client(cr<Str> ip_with_port) {
    // TODO
    error("need impl");
  }

  inline void disconnect() {
    // TODO
    error("need impl");
  }

  inline void run_packet_listening() {
    // TODO
    error("need impl");
  }

  inline void disable_packet_listening() {
    // TODO
    error("need impl");
  }

  inline bool has_packets() const {
    // TODO
    error("need impl");
    return {};
  }

  inline cr<Packets> packets() const {
    // TODO
    error("need impl");
    return {};
  }

  inline void clear_packets() {
    // TODO
    error("need impl");
  }

  inline Packet wait_packet() const {
    // TODO
    error("need impl");
    return {};
  }

  inline void send(cr<Bytes> bytes) {
    // TODO
    error("need impl");
  }

  inline void async_send(cr<Bytes> bytes, Action&& cb) {
    // TODO
    error("need impl");
  }

  inline void async_load(Packet& dst, Action&& cb) {
    // TODO
    error("need impl");
  }

  inline void update() {
    // TODO
    error("need impl");
  }
}; // Impl

Udp_mgr::Udp_mgr(): _impl{new_unique<Impl>()} {}
Udp_mgr::~Udp_mgr() {}
[[nodiscard]] bool Udp_mgr::is_server() const { return _impl->is_server(); }
[[nodiscard]] bool Udp_mgr::is_client() const { return _impl->is_client(); }
[[nodiscard]] Udp_mgr::operator bool() const { return _impl->operator bool(); }
[[nodiscard]] Strs Udp_mgr::avaliable_ipv4s() const { return _impl->avaliable_ipv4s(); }
void Udp_mgr::start_server(u16_t port) { _impl->start_server(port); }
void Udp_mgr::start_client(cr<Str> ip, u16_t port) { _impl->start_client(ip, port); }
void Udp_mgr::start_client(cr<Str> ip_with_port) { _impl->start_client(ip_with_port); }
void Udp_mgr::disconnect() { _impl->disconnect(); }
void Udp_mgr::run_packet_listening() { _impl->run_packet_listening(); }
void Udp_mgr::disable_packet_listening() { _impl->disable_packet_listening(); }
bool Udp_mgr::has_packets() const { return _impl->has_packets(); }
cr<Packets> Udp_mgr::packets() const { return _impl->packets(); }
void Udp_mgr::clear_packets() { _impl->clear_packets(); }
[[nodiscard]] Packet Udp_mgr::wait_packet() const { return _impl->wait_packet(); }
void Udp_mgr::send(cr<Bytes> bytes) { _impl->send(bytes); }
void Udp_mgr::async_send(cr<Bytes> bytes, Action&& cb) { _impl->async_send(bytes, std::move(cb)); }
void Udp_mgr::async_load(Packet& dst, Action&& cb) { _impl->async_load(dst, std::move(cb)); }
void Udp_mgr::update() { _impl->update(); }

} // net ns
