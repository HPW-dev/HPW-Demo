#include <cassert>
#include <utility>
#include "util/str-util.hpp" // перемещение вниз приведёт к взрыву
#include <asio/asio.hpp>
#include "tcp-mgr.hpp"
#include "util/error.hpp"
#include "util/log.hpp"
#include "util/platform.hpp"

namespace net {
using ip_tcp = asio::ip::tcp;

struct Tcp_mgr::Impl {
  struct Status {
    bool is_server: 1 {};
    bool is_active: 1 {};
  };
  Status _status {};
  mutable asio::io_service _io {};
  Unique<ip_tcp::socket> _socket {};
  u16_t _port {};
  Str _ip {};
  ip_tcp::endpoint _target_client_address {};
  ip_tcp::endpoint _incoming_ipv4 {}; // для ассинхронной записи туда входящего соединения
  Unique<ip_tcp::acceptor> _acceptor {};

  inline Impl() { hpw_debug("tcp-mgr created\n"); }
  inline ~Impl() { disconnect(); }
  inline bool is_server() const { return _status.is_active && _status.is_server; }
  inline bool is_client() const { return _status.is_active && !_status.is_server; }
  inline operator bool() const { return _status.is_active; }

  inline Strs avaliable_ipv4s() const {
    return_if(!this->operator bool(), {});
    ip_tcp::resolver resolver {_io};
    ip_tcp::resolver::query query(get_pc_name(), n2s(_port));
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
    init_unique(_socket, _io);
    //_socket->set_option(ip_tcp::socket::reuse_address(true));
    _incoming_ipv4 = ip_tcp::endpoint(ip_tcp::v4(), _port);
    init_unique<ip_tcp::acceptor>(_acceptor, _io, _incoming_ipv4);
    _status.is_active = true;
    _status.is_server = true;
  }

  inline void start_client(cr<Str> ip, u16_t port) {
    iferror(ip.empty(), "empty ip string");
    _ip = ip;
    _port = port;
    init_unique(_socket, _io);
    _socket->open(ip_tcp::v4());
    _target_client_address = ip_tcp::endpoint(asio::ip::address::from_string(_ip), _port);
    _status.is_active = true;
    _status.is_server = false;
    hpw_debug("UDP клиент создан и настроен на " + _ip + ":" + n2s(_port) + "\n");
  }

  inline void start_client(cr<Str> ip_with_port) {
    error("need impl");
    //start_client() parsed ip:port
  }

  inline void disconnect() {
    hpw_debug("выключение tcp-mgr\n");
    _io.stop();
    _acceptor = {};
    _socket = {};
    _status.is_active = false;
  }

  inline void update() {
    iferror(!_status.is_active, "not initialized");
    _io.run_for(std::chrono::seconds(1));
  }

  inline void async_find_incoming_ipv4(std::atomic_bool& connected, Str& dst) {
    iferror(connected, "set connected as false");
    iferror(!_status.is_active, "not initialized");
    iferror(!_status.is_server, "is not a server");
    
    auto handler = [&](cr<std::error_code> err, ip_tcp::socket socket) {
      iferror(err, err.message());
      dst = socket.remote_endpoint().address().to_v4().to_string();
      connected = true;
    };

    assert(_acceptor);
    _acceptor->async_accept(handler);
  }

  inline void async_connect(std::atomic_bool& connected) {
    iferror(connected, "set connected as false");
    iferror(!_status.is_active, "not initialized");
    iferror(_status.is_server, "is not a client");
    
    auto handler = [&](cr<std::error_code> err) {
      iferror(err, err.message());
      connected = true;
    };

    assert(_socket);
    _socket->async_connect(_target_client_address, handler);
  }
}; // Impl

Tcp_mgr::Tcp_mgr(): _impl{new_unique<Impl>()} {}
Tcp_mgr::~Tcp_mgr() {}
[[nodiscard]] bool Tcp_mgr::is_server() const { return _impl->is_server(); }
[[nodiscard]] bool Tcp_mgr::is_client() const { return _impl->is_client(); }
[[nodiscard]] Tcp_mgr::operator bool() const { return _impl->operator bool(); }
[[nodiscard]] Strs Tcp_mgr::avaliable_ipv4s() const { return _impl->avaliable_ipv4s(); }
void Tcp_mgr::start_server(u16_t port) { _impl->start_server(port); }
void Tcp_mgr::start_client(cr<Str> ip, u16_t port) { _impl->start_client(ip, port); }
void Tcp_mgr::start_client(cr<Str> ip_with_port) { _impl->start_client(ip_with_port); }
void Tcp_mgr::disconnect() { _impl->disconnect(); }
void Tcp_mgr::update() { _impl->update(); }
void Tcp_mgr::async_find_incoming_ipv4(std::atomic_bool& connected, Str& dst) { _impl->async_find_incoming_ipv4(connected, dst); }
void Tcp_mgr::async_connect(std::atomic_bool& connected) { _impl->async_connect(connected); }

} // net ns
