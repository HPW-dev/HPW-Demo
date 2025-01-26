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
  Action _if_loaded_cb {}; // действие при загрузке пакета
  ip_udp::endpoint _input_addr {}; // адрес от входящего пакета
  Packet _input_packet {}; // промежуточный пакет для записи в него входящих пакетов

  inline ~Impl() { disconnect(); }

  inline void start_server(cr<Str> ip_v4, Port port) {
    disconnect();
    hpw_debug("Udp_packet_mgr.start_server\n");

    _port = port;
    if (_port < 1024 || _port > 49'150)
      hpw_warning("use recomended UPD-ports in 1024...49'150\n");

    if (ip_v4.empty())
      _ip_v4 = MY_IPV4;
    else
      _ip_v4 = ip_v4;
    init_unique(_socket, _io, ip_udp::endpoint(asio::ip::address_v4::from_string(_ip_v4), _port));
    _socket->set_option(ip_udp::socket::reuse_address(true));
    _socket->set_option(ip_udp::socket::broadcast(true));
    _status.is_active = true;
    _status.is_server = true;
    start_waiting_packets();
  }

  inline void start_client(cr<Str> ip_v4, Port port=net::DEFAULT_PORT) {
    disconnect();
    hpw_debug("Udp_packet_mgr.start_client\n");
    _port = port;
    if (ip_v4.empty())
      _ip_v4 = MY_IPV4;
    else
      _ip_v4 = ip_v4;
    init_unique(_socket, _io, ip_udp::endpoint(asio::ip::address_v4::from_string(_ip_v4), _port));
    _status.is_active = true;
    _status.is_server = false;
    start_waiting_packets();
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

  inline void send(cr<Packet> src, cr<Str> ip_v4, const Port port) {
    iferror(!_status.is_active, "not initialized");
    iferror(src.bytes.empty(), "нет данных для оптравки");
    iferror(src.bytes.size() >= net::PACKET_BUFFER_SZ,
      "данных для отправки больше чем допустимый размер пакета");
    hpw_debug("ссинхронная отправка " + n2s(src.bytes.size()) + " байт...\n");
    ip_udp::endpoint endpoint(asio::ip::address_v4::from_string(ip_v4), port);
    assert(_socket);
    cauto sended = _socket->send_to(asio::buffer(src.bytes), endpoint);
    iferror(sended == 0, "данные не отправлены");
  }

  inline void push(cr<Packet> src, cr<Str> ip_v4, const Port port, Action&& cb) {
    iferror(!_status.is_active, "not initialized");
    iferror(src.bytes.empty(), "нет данных для оптравки");
    iferror(src.bytes.size() >= net::PACKET_BUFFER_SZ,
      "данных для отправки больше чем допустимый размер пакета");

    _packets_to_send.push_back(src);
    auto* for_delete = &_packets_to_send.back();

    auto handler = [this, _for_delete=for_delete, cb=std::move(cb)]
    (cr<std::error_code> err, std::size_t bytes) {
      return_if(!_status.is_active);

      if (err) {
        hpw_debug(Str("системная ошибка: ") + err.message() + " - " + err.category().name() + "\n");
        start_waiting_packets();
      }

      if (bytes == 0) {
        hpw_debug("данные не отправлены\n");
        start_waiting_packets();
      }

      if (bytes >= net::PACKET_BUFFER_SZ) {
        hpw_debug("недопустимый размер пакета\n");
        start_waiting_packets();
      }
      
      hpw_debug("отправлено " + n2s(bytes) + " байт\n");

      if (cb)
        cb();
        
      // удалить пакет из списка
      std::erase_if(_packets_to_send, [_for_delete](cr<Packet> packet){ return std::addressof(packet) == _for_delete; });
    };

    hpw_debug("ассинхронная отправка " + n2s(for_delete->bytes.size()) + " байт...\n");
    assert(_socket);
    ip_udp::endpoint endpoint(asio::ip::address_v4::from_string(ip_v4), port);
    _socket->async_send_to(asio::buffer(for_delete->bytes), endpoint, handler);
  }

  inline void broadcast_push(cr<Packet> src, const Port port, Action&& cb) {
    iferror(!_status.is_active, "not initialized");
    iferror(src.bytes.empty(), "нет данных для оптравки");
    iferror(src.bytes.size() >= net::PACKET_BUFFER_SZ,
      "данных для отправки больше чем допустимый размер пакета");

    _packets_to_send.push_back(src);
    auto* for_delete = &_packets_to_send.back();

    auto handler = [this, _for_delete=for_delete, cb=std::move(cb)]
    (cr<std::error_code> err, std::size_t bytes) {
      return_if(!_status.is_active);

      if (err) {
        hpw_debug(Str("системная ошибка: ") + err.message() + " - " + err.category().name() + "\n");
        start_waiting_packets();
      }

      if (bytes == 0) {
        hpw_debug("данные не отправлены\n");
        start_waiting_packets();
      }

      if (bytes >= net::PACKET_BUFFER_SZ) {
        hpw_debug("недопустимый размер пакета\n");
        start_waiting_packets();
      }

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

  inline Packets unload_all() {
    iferror(!_status.is_active, "not initialized");
    Packets ret;
    for (rauto packet: _loaded_packets)
      ret.emplace_back(std::move(packet));
    _loaded_packets.clear();
    return ret;
  }

  inline bool has_packets() const {
    iferror(!_status.is_active, "not initialized");
    return !_loaded_packets.empty();
  }

  inline cr<Port> port() const {
    iferror(!_status.is_active, "not initialized");
    return _port;
  }

  inline cr<Str> ip_v4() const {
    iferror(!_status.is_active, "not initialized");
    return _ip_v4;
  }

  // включает приём пакетов и их сохранение в буффер
  inline void start_waiting_packets() {
    iferror(!_status.is_active, "not initialized");

    // размер пакета изначально больше чем принимаемые данные
    _input_packet = {};
    _input_packet.bytes.resize(net::PACKET_BUFFER_SZ);
    
    assert(_socket);
    _socket->async_receive_from (
      asio::buffer(_input_packet.bytes),
      _input_addr,
      [this](cr<std::error_code> err, std::size_t bytes)->void { // handler
        return_if(!_status.is_active);

        if(err) {
          hpw_debug(Str("системная ошибка: ") + err.message() + " - " + err.category().name() + "\n");
          start_waiting_packets();
        }

        if(bytes == 0) {
          hpw_debug("данные не прочитаны\n");
          start_waiting_packets();
        }

        if(bytes >= net::PACKET_BUFFER_SZ) {
          hpw_debug("недопустимый размер пакета\n");
          start_waiting_packets();
        }

        if (_if_loaded_cb)
          _if_loaded_cb();

        _input_packet.bytes.resize(bytes); // сократить размер пакета
        _input_packet.ip_v4 = _input_addr.address().to_v4().to_string();
        _input_packet.port = _input_addr.port();

        // передать загруженный пакет в буффер загрузок и запустить следующий приём пакетов
        _loaded_packets.emplace_back(std::move(_input_packet));
        start_waiting_packets();
      }
    );
  }

  inline void action_if_loaded(Action&& cb) { _if_loaded_cb = std::move(cb); }
  inline bool is_server() const { return is_active() && _status.is_server; }
  inline bool is_client() const { return is_active() && !_status.is_server; }
  inline bool is_active() const { return _status.is_active; }
}; // Impl

Udp_packet_mgr::Udp_packet_mgr(): _impl{new_unique<Impl>()} {}
Udp_packet_mgr::~Udp_packet_mgr() {}
void Udp_packet_mgr::start_server(cr<Str> ip_v4, Port port) { _impl->start_server(ip_v4, port); }
void Udp_packet_mgr::start_client(cr<Str> ip, Port port) { _impl->start_client(ip, port); }
void Udp_packet_mgr::update() { _impl->update(); }
void Udp_packet_mgr::broadcast_push(cr<Packet> src, const Port port, Action&& cb)
  { _impl->broadcast_push(src, port, std::move(cb)); }
void Udp_packet_mgr::push(cr<Packet> src, cr<Str> ip_v4, const Port port, Action&& cb)
  { _impl->push(src, ip_v4, port, std::move(cb)); }
void Udp_packet_mgr::send(cr<Packet> src, cr<Str> ip_v4, const Port port)
  { _impl->send(src, ip_v4, port); }
cr<Port> Udp_packet_mgr::port() const { return _impl->port(); }
cr<Str> Udp_packet_mgr::ip_v4() const { return _impl->ip_v4(); }
bool Udp_packet_mgr::is_server() const { return _impl->is_server(); }
bool Udp_packet_mgr::is_client() const { return _impl->is_client(); }
bool Udp_packet_mgr::is_active() const { return _impl->is_active(); }
Packets Udp_packet_mgr::unload_all() { return _impl->unload_all(); }
bool Udp_packet_mgr::has_packets() const { return _impl->has_packets(); }
void Udp_packet_mgr::action_if_loaded(Action&& cb) { _impl->action_if_loaded(std::move(cb)); }

} // net ns
