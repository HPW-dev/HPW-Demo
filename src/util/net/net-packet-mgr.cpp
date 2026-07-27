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
  asio::io_service _io_udp {}; // поток для UDP соединения
  asio::io_service _io_tcp {}; // поток для TCP соединения
  Unique<ip_udp::socket> _socket_udp {};
  Unique<ip_tcp::socket> _socket_tcp {};
  std::deque<Packet> _packets_to_send {}; // пакеты ждущие отправки
  std::deque<Packet> _loaded_packets {}; // полученные пакеты
  Packet _input_packet {}; // промежуточный пакет для записи в него входящих пакетов
  ip_udp::endpoint _input_udp_addr {}; // адрес от входящего UDP пакета
  ip_tcp::endpoint _last_binded_addr {}; // последний адресс, на который биндились по TCP
  Action _receive_cb {}; // действие выполняющиеся при получении пакета

  inline void start(cr<Config> cfg) {
    disconnect();
    _check_config(cfg);
    _receive_cb = cfg.receive_cb;
    _status.server = cfg.is_server;
    _init_udp(cfg);
    _init_tcp(cfg);
    _status.active = true;
    _start_waiting_packets(true);
  }

  inline void disconnect() {
    ret_if (!_status.active);
    _status = {};
    _io_udp.stop();
    _io_tcp.stop();
    _socket_udp = {};
    _socket_tcp = {};
    _packets_to_send.clear();
    _loaded_packets.clear();
    _input_packet = {};
    _input_udp_addr = {};
    _receive_cb = {};
    log_info << "network packet manager disconnected";
  }

  inline void update() {
    iferror(!_status.active, "not initialized");
    constexpr auto TIMEOUT = std::chrono::seconds(1);
    _io_udp.run_for(TIMEOUT);
    _io_tcp.run_for(TIMEOUT);
  }

  inline void send(cr<Packet> src, cr<Target_info> target) {
    iferror(!_status.active, "not initialized");
    iferror(!target.broadcast && target.ip_v4.empty(), "target ip v4 is empty");
    iferror(src.bytes.empty(), "packet data is empty");
    iferror(src.bytes.size() >= (target.udp_mode ? net::MAX_UDP_PACKET : net::MAX_TCP_PACKET), "packet data >= MAX_PACKET");
    if (target.port != 0 && target.port < 1024)
      hpw_warning("target port < 1024");

    _packets_to_send.push_back(src);
    auto* for_delete = &_packets_to_send.back();

    auto handler = [this, _for_delete=for_delete, tgt=target]
    (cr<std::error_code> err, std::size_t bytes) {
      ret_if(!_status.active);

      if (err) {
        hpw_warning(Str("system error: ") + err.message() + " - " + err.category().name() + "\n");
      } elif (bytes == 0) {
        hpw_warning("sended 0 bytes\n");
      } elif (bytes >= (tgt.udp_mode ? net::MAX_UDP_PACKET : net::MAX_TCP_PACKET)) {
        hpw_warning("packet size >= MAX_PACKET\n");
      } else {
        hpw_debug("отправлено " + n2s(bytes) + " байт\n");
        
        if (tgt.send_cb)
          tgt.send_cb();

        ++_status.sended_packets;
      }
        
      // удалить пакет из списка
      std::erase_if(_packets_to_send, [_for_delete](cr<Packet> packet){ return std::addressof(packet) == _for_delete; });
    };

    hpw_debug("отправка " + n2s(for_delete->bytes.size()) + " байт...\n");
    asio::ip::address_v4 ip = asio::ip::address_v4::from_string(target.ip_v4);
    
    if (target.udp_mode) {
      if (target.broadcast)
        ip = asio::ip::address_v4::broadcast();

      ip_udp::endpoint ep(ip, target.port);

      if (target.async) {
        _socket_udp->async_send_to(asio::buffer(for_delete->bytes), ep, handler);
      } else {
        _socket_udp->send_to(asio::buffer(for_delete->bytes), ep);
        _packets_to_send.pop_back(); // хвост отправили сразу, он уже не нужен
      }
    } else { // TCP mode
      iferror(_last_binded_addr.address().to_v4().to_string() != target.ip_v4,
        "last connected address and target ip v4 is not equal");
      if (target.async) {
        _socket_tcp->async_send(asio::buffer(for_delete->bytes), handler);
      } else {
        cauto sended = _socket_tcp->send(asio::buffer(for_delete->bytes));
        iferror(sended == 0, "данные не отправлены");
        
        hpw_debug("отправлено " + n2s(sended) + " байт\n");
        ++_status.sended_packets;

        if (target.send_cb)
          target.send_cb();
          
        _packets_to_send.pop_back(); // хвост отправили сразу, он уже не нужен
      } // else sync
    } // else TCP mode
  }

  inline Packets unload_all() {
    iferror(!_status.active, "not initialized");
    Packets ret;
    for (rauto packet: _loaded_packets)
      ret.emplace_back(std::move(packet));
    _loaded_packets.clear();
    _status.has_packets = false;
    return ret;
  }

  inline Target_info wait_connection() {
    iferror(!_status.active, "not initialized")
    _socket_tcp->close();
    const ip_tcp::endpoint ep(asio::ip::address_v4::from_string(_status.ip_v4), _status.port_tcp);
    ip_tcp::acceptor acceptor(_io_tcp, ep);
    log_info << "waiting for TCP connection...";
    acceptor.accept(*_socket_tcp);
    _last_binded_addr = _socket_tcp->remote_endpoint();
    _status.connected = true;
    _start_waiting_packets(false);

    Target_info ret;
    ret.ip_v4 = _last_binded_addr.address().to_v4().to_string();
    ret.port = _last_binded_addr.port();
    return ret;
  }

  inline void connect_to(cr<Target_info> target) {
    iferror(!_status.active, "not initialized")
    iferror(target.ip_v4.empty(), "target ip v4 is empty");
    ip_tcp::endpoint ep(asio::ip::address_v4::from_string(target.ip_v4), target.port);
    
    if (ep == _last_binded_addr) {
      log_info << "TCP connection with " << target.ip_v4 << ":" << target.port << " already established";
      return;
    }

    log_info << "TCP connect to " + target.ip_v4 + ":" << target.port;
    _socket_tcp->connect(ep);
    _last_binded_addr = ep;
    _status.connected = true;
    _start_waiting_packets(false);
  }
  
  inline cr<Status> status() const { return _status; }
  inline ~Impl() { disconnect(); }

  inline void _check_config(cr<Packet_mgr::Config> cfg) {
    iferror (cfg.port_udp != 0 && cfg.port_udp < 1024, "use UDP port > 1024");
    iferror (cfg.port_tcp != 0 && cfg.port_tcp < 1024, "use TCP port > 1024");
    iferror (cfg.ip_v4.empty(), "ip v4 is empty");
  }

  inline void _init_udp(cr<Config> cfg) {
    cauto ep = ip_udp::endpoint(asio::ip::address_v4::from_string(cfg.ip_v4), cfg.port_udp);
    init_unique(_socket_udp, _io_udp, ep);
    _status.port_udp = _socket_udp->local_endpoint().port();
    _status.ip_v4 = _socket_udp->local_endpoint().address().to_v4().to_string();
    if (cfg.is_server)
      _socket_udp->set_option(ip_udp::socket::reuse_address(true));
    _socket_udp->set_option(ip_udp::socket::broadcast(true));
    _status.broadcast = true;
    log_info << "UDP connection setup to " + _status.ip_v4 + ":" << _status.port_udp;
  }

  inline void _init_tcp(cr<Config> cfg) {   
    cauto ep = ip_tcp::endpoint(asio::ip::address_v4::from_string(cfg.ip_v4), cfg.port_tcp);
    init_unique(_socket_tcp, _io_tcp, ep);
    _status.port_tcp = _socket_tcp->local_endpoint().port();
    if (cfg.is_server)
      _socket_tcp->set_option(ip_tcp::socket::reuse_address(true));
    log_info << "TCP connection setup to " + _status.ip_v4 + ":" << _status.port_tcp;
  }

  inline void _start_waiting_packets(bool udp_mode) {
    return_if(!udp_mode && !_status.connected);
    iferror(!_status.active, "not initialized");

    // размер пакета изначально больше чем принимаемые данные
    _input_packet = {};
    _input_packet.bytes.resize(udp_mode ? net::MAX_UDP_PACKET : net::MAX_TCP_PACKET);
    
    auto handler = [this, mode=udp_mode](cr<std::error_code> err, std::size_t bytes)->void {
      return_if(!_status.active);

      if (err) {
        log_warning << Str("system error: ") + err.message() + " - " + err.category().name();
      } elif (bytes == 0) {
        log_warning << "packet data is empty";
      } elif (bytes >= (mode ? net::MAX_UDP_PACKET : net::MAX_TCP_PACKET)) {
        log_warning << "packet data >= MAX_PACKET";
      } else {
        _input_packet.bytes.resize(bytes); // сократить размер пакета

        if (mode) {
          _input_packet.by_udp = true;
          _input_packet.ip_v4 = _input_udp_addr.address().to_v4().to_string();
          _input_packet.port = _input_udp_addr.port();
        } else {
          iferror(!_status.connected, "address not connected");
          _input_packet.by_udp = false;
          _input_packet.ip_v4 = _last_binded_addr.address().to_v4().to_string();
          _input_packet.port = _last_binded_addr.port();
        }

        // передать загруженный пакет в буффер загрузок и запустить следующий приём пакетов
        _loaded_packets.emplace_back(std::move(_input_packet));
        _input_packet = {};

        if (_receive_cb)
          _receive_cb();

        _status.has_packets = true;
        ++_status.received_packets;
      }

      _start_waiting_packets(mode);
    }; // handler

    if (udp_mode) {
      _socket_udp->async_receive_from(asio::buffer(_input_packet.bytes), _input_udp_addr, handler);
    } else {
      _socket_tcp->async_receive(asio::buffer(_input_packet.bytes), handler);
    }
  }

  inline void set_receive_cb(cr<Action> cb) { _receive_cb = cb; }
}; // Impl

Packet_mgr::Packet_mgr(): _impl{new_unique<Impl>()} {}
Packet_mgr::~Packet_mgr() {}
void Packet_mgr::start(cr<Packet_mgr::Config> cfg) { _impl->start(cfg); }
void Packet_mgr::disconnect() { _impl->disconnect(); }
void Packet_mgr::update() { _impl->update(); }
void Packet_mgr::send(cr<Packet> src, cr<Target_info> target) { _impl->send(src, target); }
Packets Packet_mgr::unload_all() { return _impl->unload_all(); }
cr<Packet_mgr::Status> Packet_mgr::status() const { return _impl->status(); }
Packet_mgr::Target_info Packet_mgr::wait_connection() { return _impl->wait_connection(); }
void Packet_mgr::connect_to(cr<Target_info> target) { _impl->connect_to(target); }
void Packet_mgr::set_receive_cb(cr<Action> cb) { _impl->set_receive_cb(cb); }

} // net ns
