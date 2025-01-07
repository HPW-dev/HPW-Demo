#pragma once
#include "net.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"

namespace net {
  
// UDP сервер
class Udp_server {
public:
  explicit Udp_server(u16_t port); // создать сервер по порту
  ~Udp_server();
  bool has_packets() const; // проверить что есть входящие пакеты
  cr<Packets> packets() const; // доступ к полученным пакетам
  void clear_packets(); // снести пакеты
  Strs avaliable_ipv4s() const; // узнать по каким ipv4 можно подконектиться
  void update(); // обновить состояние сервера, роверить входящие пакеты

private:
  struct Impl;
  Unique<Impl> _impl {};
};

} // net ns
