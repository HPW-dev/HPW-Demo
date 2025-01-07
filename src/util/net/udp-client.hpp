#pragma once
#include "net.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"

namespace net {
  
// UDP клиент
class Udp_client {
public:
  explicit Udp_client(cr<Str> ip_with_port); // создать сервер по ip:port
  Udp_client(cr<Str> ip, u16_t port); // создать сервер по ip и порту
  ~Udp_client();
  bool has_packets() const; // проверить что есть пакеты для отправки
  cr<Packets> packets() const; // доступ к пакетам
  void clear_packets(); // снести пакеты
  void try_to_connect(); // попытка подключения к серверу
  void send(cr<Bytes> bytes); // отправить данные на сервер
  void async_send(cr<Bytes> bytes); // отправить данные на сервер без блокировки потока
  void update(); // обновить состояние сервера, роверить входящие пакеты

private:
  struct Impl;
  Unique<Impl> _impl {};
};

} // net ns
