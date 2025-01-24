#pragma once
#include <functional>
#include "net.hpp"
#include "util/mem-types.hpp"
#include "util/macro.hpp"

namespace net {

// ассинхронно получает или передаёт сетевые пакеты
class Udp_packet_mgr {
public:
  using Action = std::function<void ()>;

  Udp_packet_mgr();
  virtual ~Udp_packet_mgr();
  void start_server(Port port=net::DEFAULT_PORT);
  void start_client(cr<Str> ip, Port port=net::DEFAULT_PORT);
  [[nodiscard]] bool is_server() const;
  [[nodiscard]] bool is_client() const;
  [[nodiscard]] cr<Port> port() const; // узнать свой порт
  [[nodiscard]] cr<Str> ip_v4() const; // узнать свой IPv4
  void update();
  /** широковещательная отправка пакета
  * @param src данные отправки
  * @param ip адрес получателя
  * @param port порт получателя
  * @param cb действие при завершении отправки */
  void broadcast_push(Packet&& src, const Port port=net::DEFAULT_PORT, Action&& cb={});

private:
  struct Impl;
  Unique<Impl> _impl {};
};

} // net ns
