#include <cassert>
#include <utility>
#include <deque>

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
  std::deque<Packet> _packets_to_send {}; // пакеты ждущие отправки
  std::deque<Packet> _loaded_packets {}; // полученные пакеты

  inline ~Impl() { disconnect(); }

  inline void start_server(Port port) {
    disconnect();
    hpw_debug("Udp_packet_mgr.start_server\n");

    _port = port;
    if (_port < 1024 || _port > 49'150)
      hpw_warning("use recomended UPD-ports in 1024...49'150\n");

    init_unique(_socket, _io, ip_udp::endpoint(ip_udp::v4(), _port));
    _socket->set_option(ip_udp::socket::reuse_address(true));
    _socket->set_option(ip_udp::socket::broadcast(true));
    _status.is_active = true;
    _status.is_server = true;
  }

  inline void start_client(cr<Str> ip, Port port) {
    disconnect();
    hpw_debug("Udp_packet_mgr.start_client\n");

    iferror(ip.empty(), "empty ip string");
    _ip_v4 = ip;
    _port = port;
    init_unique(_socket, _io);
    _socket->open(ip_udp::v4());
    _status.is_active = true;
    _status.is_server = false;
    hpw_debug("UDP клиент создан и настроен на " + _ip_v4 + ":" + n2s(_port) + "\n");
  }

  inline void update() {
    iferror(!_status.is_active, "not initialized");
    _io.run_for(std::chrono::seconds(1));
  }

  inline void disconnect() {
    hpw_debug("Udp_packet_mgr.disconnect\n");
    _io.stop();
    _socket = {};
    _status.is_active = false;
    _packets_to_send.clear();
    _loaded_packets.clear();
  }

  inline void broadcast_push(Packet&& src, const Port port, Action&& cb) {
    iferror(!_status.is_active, "not initialized");
    iferror(src.bytes.empty(), "нет данных для оптравки");
    iferror(src.bytes.size() >= net::PACKET_BUFFER_SZ,
      "данных для отправки больше чем допустимый размер пакета");

    auto* for_delete = &_packets_to_send.emplace_back(std::move(src));

    auto handler = [this, _for_delete=for_delete, cb=std::move(cb)]
    (cr<std::error_code> err, std::size_t bytes) {
      return_if(!_status.is_active);
      iferror(err, err.message());
      iferror(bytes == 0, "данные не отправлены");
      iferror(bytes >= net::PACKET_BUFFER_SZ, "недопустимый размер пакета");
      hpw_debug("отправлено " + n2s(bytes) + " байт\n");

      if (cb)
        cb();
        
      // удалить пакет из списка
      std::erase_if(_packets_to_send, [_for_delete](cr<Packet> packet){ return std::addressof(packet) == _for_delete; });
    };

    hpw_debug("ассинхронная отправка " + n2s(for_delete->bytes.size()) + " байт...\n");
    assert(_socket);
    ip_udp::endpoint broadcast_endpoint(asio::ip::address_v4::broadcast(), port);
    _socket->async_send_to(asio::buffer(for_delete->bytes), broadcast_endpoint, handler);
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
