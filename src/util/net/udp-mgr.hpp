#pragma once
#include <functional>
#include "net.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"

namespace net {

// управляет сетевым взаимодействием по UDP
class Udp_mgr {
public:
  using Action = std::function<void (std::size_t)>;

  Udp_mgr();
  ~Udp_mgr();
  // проверить режим сервера
  [[nodiscard]] bool is_server() const;
  // проверить режим клиента
  [[nodiscard]] bool is_client() const;
  // проверить что сеть работает
  [[nodiscard]] operator bool() const;
  // узнать какие айпишники можно дать килентам, чтобы к ним подключиться
  [[nodiscard]] Strs avaliable_ipv4s() const;
  // запустить сервер
  void start_server(u16_t port);
  // запустить клиент
  void start_client(cr<Str> ip, u16_t port);
  // запустить клиент и подключиться к ip:port
  void start_client(cr<Str> ip_with_port);
  // выключить соединение
  void disconnect();
  // включить отдельный поток ожидания пакетов. о том, что пакеты есть, можно узнать в has_packets()
  void run_packet_listening();
  // выключить поток ожидания пакетов
  void disable_packet_listening();
  // узнать что есть полученные пакеты
  bool has_packets() const;
  // список пакетов. По флагу loaded_correctly можно узнать что пакет загружен нормально
  [[nodiscard]] cr<Packets> packets() const;
  // удалить список полученных пакетов
  void clear_packets();
  // ждать получения пакета
  [[nodiscard]] Packet wait_packet() const;
  // отправить данные
  void send(cr<Bytes> bytes);
  // отправить данные без блокировки потока. Вызывать cb, когда данные будут отправлены
  void async_send(cr<Bytes> bytes, Action&& cb = {});
  /* загрузить пакет без блокировки потока. Вызывать cb, когда данные пакет будет получен,
  в пакете будет loaded_correctly = true */
  void async_load(Packet& dst, Action&& cb = {});
  // обновление ивентов системы
  void update();

private:
  struct Impl;
  Unique<Impl> _impl {};
};

} // net ns
