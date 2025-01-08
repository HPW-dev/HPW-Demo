#include <cassert>
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
  };
  Status _status {};
  mutable asio::io_service _io {};
  Unique<ip_udp::socket> _socket {};
  u16_t _port {};
  Str _ip {};
  ip_udp::endpoint _target_client_address {};

  inline Impl() { hpw_debug("udp-mgr created\n"); }
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
    iferror(ip.empty(), "empty ip string");
    _ip = ip;
    _port = port;
    init_unique(_socket, _io);
    _socket->open(ip_udp::v4());
    _target_client_address = ip_udp::endpoint(asio::ip::address::from_string(_ip), _port);
    _status.is_active = true;
    _status.is_server = false;
    hpw_debug("UDP клиент создан и настроен на " + _ip + ":" + n2s(_port) + "\n");
  }

  inline void start_client(cr<Str> ip_with_port) {
    error("need impl");
    //start_client() parsed ip:port
  }

  inline void disconnect() {
    hpw_debug("выключение udp-mgr\n");
    _io.stop();
    _socket = {};
    _status.is_active = false;
  }
  
  inline Packet load_packet() {
    iferror(!_status.is_active, "not initialized");
    Packet ret;
    ret.bytes.resize(net::PACKET_BUFFER_SZ);
    assert(_socket);
    ip_udp::endpoint address;
    cauto loaded_bytes = _socket->receive_from(asio::buffer(ret.bytes), address);
    iferror(loaded_bytes >= net::PACKET_BUFFER_SZ, "превышение размера получаемого пакета");
    iferror(loaded_bytes == 0, "данные не получены");
    ret.source_address = address.address().to_string();
    ret.bytes.resize(loaded_bytes);
    return ret;
  }

  inline void send(cr<Bytes> bytes, cr<Str> ip, std::optional<u16_t> port) {
    iferror(!_status.is_active, "not initialized");
    iferror(bytes.empty(), "нет данных для передачи");
    iferror(bytes.size() >= net::PACKET_BUFFER_SZ, "превышение размера передаваемого пакета");
    assert(_socket);

    hpw_debug("отправка " + n2s(bytes.size()) + " байт...\n");
    std::size_t sended_bytes;
    if (!_status.is_server) {
      sended_bytes = _socket->send_to(asio::buffer(bytes), _target_client_address);
      hpw_debug("адрес назначения: " + _target_client_address.address().to_string() + "\n");
    } else {
      iferror(ip.empty(), "ip пустой");
      const u16_t cur_port = port.has_value() ? port.value() : _port;
      ip_udp::endpoint target_address(asio::ip::address::from_string(ip), cur_port);
      sended_bytes = _socket->send_to(asio::buffer(bytes), target_address);
      hpw_debug("адрес назначения: " + target_address.address().to_string() + "\n");
    }
    
    iferror(sended_bytes == 0, "данные не переданы");
    iferror(sended_bytes >= net::PACKET_BUFFER_SZ, "недопустимый размер пакета");
    hpw_debug(n2s(sended_bytes) + " байт было отправлено\n");
  }

  inline void async_send(cr<Bytes> bytes, Action&& cb, cr<Str> ip, std::optional<u16_t> port) {
    iferror(!_status.is_active, "not initialized");
    iferror(bytes.empty(), "нет данных для оптравки");
    iferror(bytes.size() >= net::PACKET_BUFFER_SZ, "данных для отправки больше чем допустимый размер пакета");

    auto handler = [&, cb=cb](cr<std::error_code> err, std::size_t bytes)->void {
      return_if(!_status.is_active);
      iferror(err, err.message());
      iferror(bytes == 0, "данные не прочитаны");
      iferror(bytes >= net::PACKET_BUFFER_SZ, "недопустимый размер пакета");
      if (cb)
        cb(bytes);
      hpw_debug("отправлено " + n2s(bytes) + " байт\n");
    };

    hpw_debug("ассинхронная отправка " + n2s(bytes.size()) + " байт...\n");
    assert(_socket);
    if (!_status.is_server) {
      _socket->async_send_to(asio::buffer(bytes), _target_client_address, handler);
    } else {
      iferror(ip.empty(), "ip пустой");
      const u16_t cur_port = port.has_value() ? port.value() : _port;
      ip_udp::endpoint target_address(asio::ip::address::from_string(ip), cur_port);
      _socket->async_send_to(asio::buffer(bytes), target_address, handler);
    }
  }

  inline void async_load(Packet& dst, Action&& cb) {
    iferror(_status.is_active, "not initialized");
    auto address = new_shared<ip_udp::endpoint>();
    
    auto handler = [&, cb=cb, address=address](cr<std::error_code> err, std::size_t bytes)->void {
      return_if(!_status.is_active);
      iferror(err, err.message());
      iferror(bytes == 0, "данные не прочитаны");
      iferror(bytes >= net::PACKET_BUFFER_SZ, "недопустимый размер пакета");
      if (cb)
        cb(bytes);
      dst.bytes.resize(bytes);
      assert(address);
      dst.source_address = address->address().to_string();
    };

    assert(_socket);
    dst.bytes.resize(net::PACKET_BUFFER_SZ);
    dst.source_address.clear();
    _socket->async_receive_from(asio::buffer(dst.bytes), *address, handler);
  }

  inline void update() {
    iferror(!_status.is_active, "not initialized");
    _io.run();
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
[[nodiscard]] Packet Udp_mgr::load_packet() { return _impl->load_packet(); }
void Udp_mgr::send(cr<Bytes> bytes, cr<Str> ip, std::optional<u16_t> port) { _impl->send(bytes, ip, port); }
void Udp_mgr::async_send(cr<Bytes> bytes, Action&& cb, cr<Str> ip, std::optional<u16_t> port)
  { _impl->async_send(bytes, std::move(cb), ip, port); }
void Udp_mgr::async_load(Packet& dst, Action&& cb) { _impl->async_load(dst, std::move(cb)); }
void Udp_mgr::update() { _impl->update(); }

} // net ns
