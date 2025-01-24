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
  /** широковещательная отправка пакета
  * @param src данные отправки
  * @param ip адрес получателя
  * @param port порт получателя
  * @param cb действие при завершении отправки */
  void broadcast_push(cr<Packet> src, cr<Str> ip, const u16_t port, Udp_mgr::Action&& cb={});

private:
  struct Impl;
  Unique<Impl> _impl {};
};

} // net ns
