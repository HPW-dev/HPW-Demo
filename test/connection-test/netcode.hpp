#pragma once
#include "util/mem-types.hpp"
#include "util/net/net.hpp"

class Image;

struct Connection_ctx {
  Str ip_v4 {net::SELF_IPV4};
  net::Port udp_server {net::UDP_SERVER_PORT};
  net::Port tcp_server {net::TCP_SERVER_PORT};
  net::Port udp_client {net::UDP_CLIENT_PORT};
  net::Port tcp_client {net::TCP_CLIENT_PORT};
};

// рулит сетевым взаимодействием в этом тесте
class Netcode final {
public:
  explicit Netcode(bool is_server, cr<Connection_ctx> ctx);
  ~Netcode();
  // если есть широковещательное приглашение, то законектиться к нему
  void connect_to_broadcast();
  // зайти по айпишнику
  void connect_to(cr<Str> ip_v4);
  void draw(Image& dst) const;
  void update(const Delta_time dt);

private:
  struct Impl;
  Unique<Impl> _impl {};
};
