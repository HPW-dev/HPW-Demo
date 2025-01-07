#include "util/str-util.hpp"
#include <asio/asio.hpp>
#include "udp-server.hpp"
#include "util/log.hpp"
#include "util/platform.hpp"

namespace net {
using ip_udp = asio::ip::udp;

struct Udp_server::Impl {
  u16_t _port {};
  Packets _packets {};
  mutable asio::io_service _io {};

  inline explicit Impl(u16_t port)
  : _port {port}
  {
    if (_port < 1024 || _port > 49'150)
      hpw_warning("use recomended UPD-ports in 1024...49'150\n");
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
    ret.push_back("127.0.0.1:" + port);
    return ret;
  }
};

Udp_server::Udp_server(u16_t port): _impl{new_unique<Impl>(port)} {}
Udp_server::~Udp_server() {}
bool Udp_server::has_packets() const { return _impl->has_packets(); }
cr<Packets> Udp_server::packets() const { return _impl->packets(); }
void Udp_server::clear_packets() { _impl->clear_packets(); }
Strs Udp_server::avaliable_ipv4s() const { return _impl->avaliable_ipv4s(); }

} // net ns
