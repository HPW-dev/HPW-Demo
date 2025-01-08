#pragma once
#include <atomic>
#include <functional>
#include <optional>
#include "net.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"

namespace net {

// управляет сетевым взаимодействием по TCP
class Tcp_mgr {
public:
  using Action = std::function<void (std::size_t)>;

  Tcp_mgr();
  ~Tcp_mgr();
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
  // обновление ивентов системы
  void update();
  /* ассинхронно ожидать подключение и записать адрес в dst.
  Через connected можно понять что адрес получен */
  void async_find_incoming_ipv4(std::atomic_bool& connected, Str& dst);
  // поытка ассинхроннного подключения к серверу, connected станет true при успехе
  void async_connect(std::atomic_bool& connected);

private:
  struct Impl;
  Unique<Impl> _impl {};
};

} // net ns
