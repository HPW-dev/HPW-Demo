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
  struct Status {
    bool is_server: 1 {};
    bool is_active: 1 {};
  };

  Status _status {};
  mutable asio::io_service _udp_io {};
  mutable asio::io_service _tcp_io {};
  Unique<ip_udp::socket> _udp_socket {};
  Unique<ip_tcp::socket> _tcp_socket {};
  Port _udp_port {net::AUTO_PORT};
  Port _tcp_port {net::AUTO_PORT};
  Str _ip_v4 {net::SELF_IPV4}; // свой IPv4
  std::deque<Packet> _packets_to_send {}; // пакеты ждущие отправки
  std::deque<Packet> _loaded_packets {}; // полученные пакеты
  Action _receive_cb {}; // действие при загрузке пакета
  ip_udp::endpoint _input_udp_addr {}; // адрес от входящего UDP пакета
  std::optional<ip_tcp::endpoint> _last_tcp_addr {}; // адрес последнего бинда TCP
  Packet _input_packet {}; // промежуточный пакет для записи в него входящих пакетов
  uint _received {}; // сколько пакетов получено
  uint _sended {}; // сколько пакетов отправлено

  inline ~Impl() { disconnect(); }

  inline void start_server(cr<Str> ip_v4, Port udp_port, Port tcp_port) {
    disconnect();
    hpw_debug("Packet_mgr.start_server\n");

    if (udp_port < 1024 || tcp_port < 1024)
      hpw_warning("лучше использовать порты больше чем 1024\n");

    if (ip_v4.empty())
      _ip_v4 = SELF_IPV4;
    else
      _ip_v4 = ip_v4;

    cauto tmp_ip = asio::ip::address_v4::from_string(_ip_v4);
    init_unique(_udp_socket, _udp_io, ip_udp::endpoint(tmp_ip, udp_port));
    init_unique(_tcp_socket, _tcp_io, ip_tcp::endpoint(tmp_ip, tcp_port));
    _ip_v4 = _tcp_socket->local_endpoint().address().to_v4().to_string();
    _udp_port = _udp_socket->local_endpoint().port();
    _tcp_port = _tcp_socket->local_endpoint().port();
    _udp_socket->set_option(ip_udp::socket::reuse_address(true));
    _tcp_socket->set_option(ip_udp::socket::reuse_address(true));
    _udp_socket->set_option(ip_udp::socket::broadcast(true));
    _tcp_socket->set_option(ip_udp::socket::broadcast(true));
    _status.is_active = true;
    _status.is_server = true;
    start_waiting_packets(true);
    start_waiting_packets(false);
    hpw_debug("UDP сервер создан и настроен на " + _ip_v4 + ":" + n2s(_udp_port) + "\n");
    hpw_debug("TCP сервер создан и настроен на " + _ip_v4 + ":" + n2s(_tcp_port) + "\n");
  }

  inline void start_client(cr<Str> ip_v4, Port udp_port, Port tcp_port) {
    disconnect();
    hpw_debug("Packet_mgr.start_client\n");
    
    if (udp_port < 1024 || tcp_port < 1024)
      hpw_warning("лучше использовать порты больше чем 1024\n");

    if (ip_v4.empty())
      _ip_v4 = SELF_IPV4;
    else
      _ip_v4 = ip_v4;

    cauto tmp_ip = asio::ip::address_v4::from_string(_ip_v4);
    init_unique(_udp_socket, _udp_io, ip_udp::endpoint(tmp_ip, udp_port));
    init_unique(_tcp_socket, _tcp_io, ip_tcp::endpoint(tmp_ip, tcp_port));
    _ip_v4 = _tcp_socket->local_endpoint().address().to_v4().to_string();
    _udp_port = _udp_socket->local_endpoint().port();
    _tcp_port = _tcp_socket->local_endpoint().port();
    _status.is_active = true;
    _status.is_server = false;
    start_waiting_packets(true);
    start_waiting_packets(false);
    hpw_debug("UDP клиент создан и настроен на " + _ip_v4 + ":" + n2s(_udp_port) + "\n");
    hpw_debug("TCP клиент создан и настроен на " + _ip_v4 + ":" + n2s(_tcp_port) + "\n");
  }

  inline void update() {
    iferror(!_status.is_active, "not initialized");
    _udp_io.run_for(std::chrono::seconds(1));
    _tcp_io.run_for(std::chrono::seconds(1));
  }

  inline void disconnect() {
    hpw_debug("Packet_mgr.disconnect\n");
    _udp_io.stop();
    _tcp_io.stop();
    _udp_socket = {};
    _tcp_socket = {};
    _status.is_active = false;
    _packets_to_send.clear();
    _loaded_packets.clear();
    _received = _sended = 0;
  }

  inline void send(cr<Packet> src, cr<Str> ip_v4, Port port, bool udp_mode) {
    iferror(!_status.is_active, "not initialized");
    iferror(_udp_socket, "_udp_socket is null");
    iferror(_tcp_socket, "_tcp_socket is null");
    iferror(src.bytes.empty(), "нет данных для оптравки");
    iferror(src.bytes.size() >= (udp_mode ? net::MAX_UDP_PACKET : net::MAX_TCP_PACKET),
      "данных для отправки больше чем допустимый размер пакета");
    hpw_debug("синхронная отправка " + n2s(src.bytes.size()) + " байт...\n");

    std::size_t sended {};
    if (udp_mode) {
      ip_udp::endpoint endpoint(asio::ip::address_v4::from_string(ip_v4), port);
      sended = _udp_socket->send_to(asio::buffer(src.bytes), endpoint);
    } else {
      ip_tcp::endpoint endpoint(asio::ip::address_v4::from_string(ip_v4), port);
      if (_last_tcp_addr != endpoint) {
        _tcp_socket->bind(endpoint);
        _last_tcp_addr = endpoint;
      }
      sended = _tcp_socket->send(asio::buffer(src.bytes));
    }

    iferror(sended == 0, "данные не отправлены");
    ++_sended;
  }

  inline void push(cr<Packet> src, cr<Str> ip_v4, Port port, bool udp_mode, Action&& cb) {
    iferror(!_status.is_active, "not initialized");
    iferror(_udp_socket, "_udp_socket is null");
    iferror(_tcp_socket, "_tcp_socket is null");
    iferror(src.bytes.empty(), "нет данных для оптравки");
    iferror(src.bytes.size() >= (udp_mode ? net::MAX_UDP_PACKET : net::MAX_TCP_PACKET),
      "данных для отправки больше чем допустимый размер пакета");

    _packets_to_send.push_back(src);
    auto* for_delete = &_packets_to_send.back();

    auto handler = [this, mode=udp_mode, _for_delete=for_delete, cb=std::move(cb)]
    (cr<std::error_code> err, std::size_t bytes) {
      return_if(!_status.is_active);

      if (err) {
        hpw_debug(Str("системная ошибка: ") + err.message() + " - " + err.category().name() + "\n");
      } elif (bytes == 0) {
        hpw_debug("данные не отправлены\n");
      } elif (bytes >= (mode ? net::MAX_UDP_PACKET : net::MAX_TCP_PACKET)) {
        hpw_debug("недопустимый размер пакета\n");
      } else {
        hpw_debug("отправлено " + n2s(bytes) + " байт\n");

        if (cb)
          cb();
        
        ++_sended;
      }
        
      // удалить пакет из списка
      std::erase_if(_packets_to_send, [_for_delete](cr<Packet> packet){ return std::addressof(packet) == _for_delete; });
    };

    hpw_debug("ассинхронная отправка " + n2s(for_delete->bytes.size()) + " байт...\n");
    
    if (udp_mode) {
      ip_udp::endpoint endpoint(asio::ip::address_v4::from_string(ip_v4), port);
      _udp_socket->async_send_to(asio::buffer(for_delete->bytes), endpoint, handler);
    } else {
      ip_tcp::endpoint endpoint(asio::ip::address_v4::from_string(ip_v4), port);
      if (_last_tcp_addr != endpoint) {
        _tcp_socket->bind(endpoint);
        _last_tcp_addr = endpoint;
      }
      _tcp_socket->async_send(asio::buffer(for_delete->bytes), handler);
    }
  }

  inline void broadcast_push(cr<Packet> src, Port port, bool udp_mode, Action&& cb) {
    iferror(!_status.is_active, "not initialized");
    iferror(_udp_socket, "_udp_socket is null");
    iferror(_tcp_socket, "_tcp_socket is null");
    iferror(src.bytes.empty(), "нет данных для оптравки");
    iferror(src.bytes.size() >= (udp_mode ? net::MAX_UDP_PACKET : net::MAX_TCP_PACKET),
      "данных для отправки больше чем допустимый размер пакета");

    _packets_to_send.push_back(src);
    auto* for_delete = &_packets_to_send.back();

    auto handler = [this, mode=udp_mode, _for_delete=for_delete, cb=std::move(cb)]
    (cr<std::error_code> err, std::size_t bytes) {
      return_if(!_status.is_active);

      if (err) {
        hpw_debug(Str("системная ошибка: ") + err.message() + " - " + err.category().name() + "\n");
      } elif (bytes == 0) {
        hpw_debug("данные не отправлены\n");
      } elif (bytes >= (mode ? net::MAX_UDP_PACKET : net::MAX_TCP_PACKET)) {
        hpw_debug("недопустимый размер пакета\n");
      } else {
        hpw_debug("отправлено " + n2s(bytes) + " байт\n");
        
        if (cb)
          cb();

        ++_sended;
      }
        
      // удалить пакет из списка
      std::erase_if(_packets_to_send, [_for_delete](cr<Packet> packet){ return std::addressof(packet) == _for_delete; });
    };

    hpw_debug("ассинхронная отправка бродскаста " + n2s(for_delete->bytes.size()) + " байт...\n");
    
    if (udp_mode) {
      ip_udp::endpoint ep(asio::ip::address_v4::broadcast(), port);
      _udp_socket->async_send_to(asio::buffer(for_delete->bytes), ep, handler);
    } else {
      ip_tcp::endpoint ep(asio::ip::address_v4::broadcast(), port);
      if (_last_tcp_addr != ep) {
        _tcp_socket->bind(ep);
        _last_tcp_addr = ep;
      }
      _tcp_socket->async_send(asio::buffer(for_delete->bytes), handler);
    }
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

  inline cr<Port> udp_port() const {
    iferror(!_status.is_active, "not initialized");
    return _udp_port;
  }

  inline cr<Port> tcp_port() const {
    iferror(!_status.is_active, "not initialized");
    return _tcp_port;
  }

  inline cr<Str> ip_v4() const {
    iferror(!_status.is_active, "not initialized");
    return _ip_v4;
  }

  // включает приём пакетов и их сохранение в буффер
  inline void start_waiting_packets(bool udp_mode) {
    return_if(!_last_tcp_addr);
    iferror(!_status.is_active, "not initialized");
    iferror(_udp_socket, "_udp_socket is null");
    iferror(_tcp_socket, "_tcp_socket is null");

    // размер пакета изначально больше чем принимаемые данные
    _input_packet = {};
    _input_packet.bytes.resize(udp_mode ? net::MAX_UDP_PACKET : net::MAX_TCP_PACKET);
    
    auto handler = [this, mode=udp_mode](cr<std::error_code> err, std::size_t bytes)->void {
      return_if(!_status.is_active);

      if (err) {
        hpw_debug(Str("системная ошибка: ") + err.message() + " - " + err.category().name() + "\n");
      } elif (bytes == 0) {
        hpw_debug("данные не прочитаны\n");
      } elif (bytes >= (mode ? net::MAX_UDP_PACKET : net::MAX_TCP_PACKET)) {
        hpw_debug("недопустимый размер пакета\n");
      } else {
        _input_packet.bytes.resize(bytes); // сократить размер пакета

        if (mode) {
          _input_packet.ip_v4 = _input_udp_addr.address().to_v4().to_string();
          _input_packet.port = _input_udp_addr.port();
        } else {
          iferror(!_last_tcp_addr, "_last_tcp_addr is empty");
          _input_packet.ip_v4 = _last_tcp_addr->address().to_v4().to_string();
          _input_packet.port = _last_tcp_addr->port();
        }

        // передать загруженный пакет в буффер загрузок и запустить следующий приём пакетов
        _loaded_packets.emplace_back(std::move(_input_packet));
        _input_packet = {};

        if (_receive_cb)
          _receive_cb();

        ++_received;
      }

      start_waiting_packets(mode);
    }; // handler

    if (udp_mode) {
      _udp_socket->async_receive_from(asio::buffer(_input_packet.bytes), _input_udp_addr, handler);
    } else {
      _tcp_socket->async_receive(asio::buffer(_input_packet.bytes), handler);
    }
  }

  inline void set_receive_cb(Action&& cb) { _receive_cb = std::move(cb); }
  inline bool is_server() const { return is_active() && _status.is_server; }
  inline bool is_client() const { return is_active() && !_status.is_server; }
  inline bool is_active() const { return _status.is_active; }
  inline uint received_packets() const { return _received; }
  inline uint sended_packets() const { return _sended; }
}; // Impl

Packet_mgr::Packet_mgr(): _impl{new_unique<Impl>()} {}
Packet_mgr::~Packet_mgr() {}
void Packet_mgr::start_server(cr<Str> ip_v4, Port udp_port, Port tcp_port) { _impl->start_server(ip_v4, udp_port, tcp_port); }
void Packet_mgr::start_client(cr<Str> ip_v4, Port udp_port, Port tcp_port) { _impl->start_client(ip_v4, udp_port, tcp_port); }
void Packet_mgr::update() { _impl->update(); }
void Packet_mgr::broadcast_push(cr<Packet> src, Port port, bool udp_mode, Action&& cb)
  { _impl->broadcast_push(src, port, udp_mode, std::move(cb)); }
void Packet_mgr::push(cr<Packet> src, cr<Str> ip_v4, Port port, bool udp_mode, Action&& cb)
  { _impl->push(src, ip_v4, port, udp_mode, std::move(cb)); }
void Packet_mgr::send(cr<Packet> src, cr<Str> ip_v4, Port port, bool udp_mode)
  { _impl->send(src, ip_v4, port, udp_mode); }
cr<Port> Packet_mgr::udp_port() const { return _impl->udp_port(); }
cr<Port> Packet_mgr::tcp_port() const { return _impl->tcp_port(); }
cr<Str> Packet_mgr::ip_v4() const { return _impl->ip_v4(); }
bool Packet_mgr::is_server() const { return _impl->is_server(); }
bool Packet_mgr::is_client() const { return _impl->is_client(); }
bool Packet_mgr::is_active() const { return _impl->is_active(); }
Packets Packet_mgr::unload_all() { return _impl->unload_all(); }
bool Packet_mgr::has_packets() const { return _impl->has_packets(); }
void Packet_mgr::set_receive_cb(Action&& cb) { _impl->set_receive_cb(std::move(cb)); }
uint Packet_mgr::received_packets() const { return _impl->received_packets(); }
uint Packet_mgr::sended_packets() const { return _impl->sended_packets(); }

} // net ns
