#pragma once
#include "udp-mgr.hpp"

namespace net {

// ассинхронно получает или передаёт сетевые пакеты
class Udp_packet_mgr: public Udp_mgr {
public:
  Udp_packet_mgr();
  ~Udp_packet_mgr();
  void start_server(u16_t port) override;
  void start_client(cr<Str> ip, u16_t port) override;
  void start_client(cr<Str> ip_with_port) override;
  void disconnect() override;
  void update() override;

private:
  struct Impl;
  Unique<Impl> _impl {};
};

} // net ns
