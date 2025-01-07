#include <utility>
#include "util/str-util.hpp" // перемещение вниз приведёт к взрыву
#include <asio/asio.hpp>
#include "udp-mgr.hpp"
#include "util/error.hpp"
#include "util/log.hpp"
#include "util/platform.hpp"

namespace net {
using ip_udp = asio::ip::udp;

struct Udp_mgr::Impl {
  struct Status {
    bool is_server: 1 {};
    bool is_active: 1 {};
    bool is_listening_enabled: 1 {};
  };
  Status _status {};
  Packets _packets {};
  mutable asio::io_service _io {};
  Unique<ip_udp::socket> _socket {};
  u16_t _port {};
  Str _ip {};

  inline ~Impl() { disconnect(); }
  inline bool is_server() const { return _status.is_active && _status.is_server; }
  inline bool is_client() const { return _status.is_active && !_status.is_server; }
  inline operator bool() const { return _status.is_active; }

  inline Strs avaliable_ipv4s() const {
    return_if(!this->operator bool(), {});
    ip_udp::resolver resolver {_io};
    ip_udp::resolver::query query(get_pc_name(), n2s(_port));
    Strs ret;
    cauto port = n2s(_port);
    for (crauto it: resolver.resolve(query))
      if (it.endpoint().address().is_v4())
        ret.push_back(it.endpoint().address().to_string() + ":" + port);
    return ret;
  }

  inline void start_server(u16_t port) {
    _port = port;
    if (_port < 1024 || _port > 49'150)
      hpw_warning("use recomended UPD-ports in 1024...49'150\n");
    init_unique(_socket, _io, ip_udp::endpoint(ip_udp::v4(), _port));
    _socket->set_option(ip_udp::socket::reuse_address(true));
    _status.is_active = true;
    _status.is_server = true;
  }

  inline void start_client(cr<Str> ip, u16_t port) {
    _status.is_active = true;
    _status.is_server = false;
  }

  inline void start_client(cr<Str> ip_with_port) {
    _status.is_active = true;
    _status.is_server = false;
  }

  inline void disconnect() {
    disable_packet_listening();
    _io.stop();
    _socket = {};
    _status.is_active = false;
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

  inline cr<Packets> packets() const { return _packets; }

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
