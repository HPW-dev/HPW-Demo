#pragma once
#include <functional>
#include "net.hpp"
#include "util/mem-types.hpp"

namespace net {

// ассинхронно получает или передаёт сетевые пакеты
class Packet_mgr {
public:
  using Action = std::function<void ()>;

  Packet_mgr();
  virtual ~Packet_mgr();
  void start_server(cr<Str> ip=net::SELF_IPV4, Port udp_port=net::UDP_SERVER_PORT, Port tcp_port=net::TCP_SERVER_PORT);
  void start_client(cr<Str> ip=net::SELF_IPV4, Port udp_port=net::TCP_CLIENT_PORT, Port tcp_port=net::TCP_CLIENT_PORT);
  [[nodiscard]] bool is_server() const;
  [[nodiscard]] bool is_client() const;
  [[nodiscard]] bool is_active() const;
  [[nodiscard]] cr<Port> udp_port() const; // узнать свой UDP порт
  [[nodiscard]] cr<Port> tcp_port() const; // узнать свой TCP порт
  [[nodiscard]] cr<Str> ip_v4() const; // узнать свой IPv4
  [[nodiscard]] uint received_packets() const; // узнать сколько пакетов получено
  [[nodiscard]] uint sended_packets() const; // узнать сколько пакетов отправлено
  [[nodiscard]] Packets unload_all(); // выгрузить все скачанные пакеты
  [[nodiscard]] bool has_packets() const; // проверить что есть входящие пакеты
  void set_receive_cb(Action&& cb); // задать действие выполняющиеся при получении пакета
  void update(); // это обязательно вызывать, ассинорхонные отправки завершались
  /** асинхронная широковещательная отправка пакета
  * @param src данные отправки
  * @param port порт получателя
  * @param cb действие при завершении отправки */
  void broadcast_push(cr<Packet> src, Port port, Action&& cb={});
  /** асинхронная отправка пакета
  * @param src данные отправки
  * @param ip_v4 адрес получателя
  * @param port порт получателя
  * @param udp_mode true - использовать UDP, false - TCP
  * @param cb действие при завершении отправки */
  void push(cr<Packet> src, cr<Str> ip_v4, Port port, bool udp_mode, Action&& cb={});
  /** синхронная отправка пакета
  * @param src данные отправки
  * @param ip_v4 адрес получателя
  * @param port порт получателя
  * @param udp_mode true - использовать UDP, false - TCP
  * @param cb действие при завершении отправки */
  void send(cr<Packet> src, cr<Str> ip_v4, Port port, bool udp_mode);

private:
  struct Impl;
  Unique<Impl> _impl {};
};

} // net ns
