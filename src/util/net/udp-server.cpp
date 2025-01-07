#include "udp-server.hpp"
#include "util/log.hpp"

namespace net {

struct Udp_server::Impl {
  u16_t _port {};
  Packets _packets {};

  inline explicit Impl(u16_t port)
  : _port {port}
  {
    if (_port < 1024 || _port > 49'150)
      hpw_warning("use recomended UPD-ports in 1024...49'150\n");
  }

  inline bool has_packets() const { return !_packets.empty(); }
  inline cr<Packets> packets() const { return _packets; }
  inline void clear_packets() { _packets.clear(); }
};

Udp_server::Udp_server(u16_t port): _impl{new_unique<Impl>(port)} {}
Udp_server::~Udp_server() {}
bool Udp_server::has_packets() const { return _impl->has_packets(); }
cr<Packets> Udp_server::packets() const { return _impl->packets(); }
void Udp_server::clear_packets() { _impl->clear_packets(); }

} // net ns
