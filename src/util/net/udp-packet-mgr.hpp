#pragma once
#include <functional>
#include "net.hpp"
#include "util/mem-types.hpp"

namespace net {

// ассинхронно получает или передаёт сетевые пакеты
class Udp_packet_mgr {
public:
  using Action = std::function<void ()>;

  Udp_packet_mgr();
  virtual ~Udp_packet_mgr();
  void start_server(cr<Str> ip={}, Port port=net::DEFAULT_PORT);
  void start_client(cr<Str> ip={}, Port port=net::DEFAULT_PORT);
  [[nodiscard]] bool is_server() const;
  [[nodiscard]] bool is_client() const;
  [[nodiscard]] bool is_active() const;
  [[nodiscard]] cr<Port> port() const; // узнать свой порт
  [[nodiscard]] cr<Str> ip_v4() const; // узнать свой IPv4
  void update();
  /** асинхронная широковещательная отправка пакета
  * @param src данные отправки
  * @param port порт получателя
  * @param cb действие при завершении отправки */
  void broadcast_push(Packet&& src, const Port port=net::DEFAULT_PORT, Action&& cb={});
  /** асинхронная отправка пакета
  * @param src данные отправки
  * @param ip_v4 адрес получателя
  * @param port порт получателя
  * @param cb действие при завершении отправки */
  void push(Packet&& src, cr<Str> ip_v4, const Port port=net::DEFAULT_PORT, Action&& cb={});
  /** синхронная отправка пакета
  * @param src данные отправки
  * @param ip_v4 адрес получателя
  * @param port порт получателя
  * @param cb действие при завершении отправки */
  void send(Packet&& src, cr<Str> ip_v4, const Port port=net::DEFAULT_PORT);
  [[nodiscard]] Packets unload_all(); // выгрузить все скачанные пакеты
  [[nodiscard]] bool has_packets() const; // проверить что есть входящие пакеты
  void action_if_loaded(Action&& cb); // задать действие выполняющиеся при получении пакета

private:
  struct Impl;
  Unique<Impl> _impl {};
};

} // net ns
