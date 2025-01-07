#include "util/str-util.hpp"
#include <asio/asio.hpp>
#include <atomic>
#include <algorithm>
#include <cstring>
#include "udp-server.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/platform.hpp"

namespace net {
using ip_udp = asio::ip::udp;

struct Udp_server::Impl {
  u16_t _port {};
  Packets _packets {};
  mutable asio::io_service _io {};
  Unique<ip_udp::socket> _socket {};
  std::atomic_bool _live {true};
  Bytes _input_buffer {};
  Bytes _output_buffer {};
  ip_udp::endpoint _incoming_address {};

  inline explicit Impl(u16_t port)
  : _port {port}
  , _input_buffer(net::PACKET_BUFFER_SZ)
  , _output_buffer(net::PACKET_BUFFER_SZ)
  {
    if (_port < 1024 || _port > 49'150)
      hpw_warning("use recomended UPD-ports in 1024...49'150\n");
    
    hpw_debug(Str("init server by port \"") + n2s(_port) + "\"...\n");
    init_unique(_socket, _io, ip_udp::endpoint(ip_udp::v4(), _port));
    _socket->set_option(ip_udp::socket::reuse_address(true));

    auto handler = [this](cr<std::error_code> err, std::size_t bytes)->void
      { this->input_handler(err, bytes); };
    _socket->async_receive_from(asio::buffer(_input_buffer), _incoming_address, handler);
  }

  inline ~Impl() {
    _live = false;
    _io.stop();
  }

  inline bool has_packets() const { return !_packets.empty(); }
  inline cr<Packets> packets() const { return _packets; }
  inline void clear_packets() { _packets.clear(); }

  inline Strs avaliable_ipv4s() const {
    ip_udp::resolver resolver {_io};
    ip_udp::resolver::query query(get_pc_name(), n2s(_port));
    Strs ret;
    cauto port = n2s(_port);
    for (crauto it: resolver.resolve(query))
      if (it.endpoint().address().is_v4())
        ret.push_back(it.endpoint().address().to_string() + ":" + port);
    return ret;
  }

  inline void update() {
    return_if (!_live);
    _io.run();
  }

  inline void input_handler(cr<std::error_code> err, std::size_t bytes) {
    return_if(!_live);
    iferror(err, err.message());
    hpw_log("loaded bytes" + n2s(bytes) + "\n");
    return_if(bytes == 0);
    iferror(bytes >= net::PACKET_BUFFER_SZ, "превышен размер пакета ("
      << bytes << "/" << net::PACKET_BUFFER_SZ << ")");
    Packet packet;
    packet.source_address = this->_incoming_address.address().to_string();
    packet.bytes.resize(bytes);
    std::memcpy(ptr2ptr<void*>(packet.bytes.data()), cptr2ptr<cp<void>>(this->_input_buffer.data()), bytes);
    _packets.emplace_back(std::move(packet));
  }
};

Udp_server::Udp_server(u16_t port): _impl{new_unique<Impl>(port)} {}
Udp_server::~Udp_server() {}
bool Udp_server::has_packets() const { return _impl->has_packets(); }
cr<Packets> Udp_server::packets() const { return _impl->packets(); }
void Udp_server::clear_packets() { _impl->clear_packets(); }
Strs Udp_server::avaliable_ipv4s() const { return _impl->avaliable_ipv4s(); }
void Udp_server::update() { _impl->update(); }

} // net ns
