#pragma once
#include <functional>
#include "net.hpp"
#include "util/mem-types.hpp"

namespace net {

// для передачи пакетов по сети
class Packet_mgr {
public:
  // для колбэков
  using Action = std::function<void ()>;

  // конфигурация при запуске
  struct Config {
    Str ip_v4 {net::SELF_IPV4}; // какой IP назначить себе
    net::Port port_udp {net::SERVER_UDP_PORT}; // порт для UDP потока
    net::Port port_tcp {net::SERVER_TCP_PORT}; // порт для TCP потока
    bool is_server {true}; // в режиме сервера работает бродкаст
    Action receive_cb {}; // действие выполняющиеся при получении пакета
  };

  // информация о назначении пакетов
  struct Target_info {
    Str ip_v4 {}; // IP назначения (при бродкасте можно не писать)
    net::Port port {};
    Action send_cb {}; // действие при завершении отправки пакета
    bool udp_mode {};
    bool broadcast {}; // широковещательная отправка
    bool async {}; // ассинхронный режим отправки без дожидания завершения (потом вызывай .update())
  };

  // флаги
  struct Status {
    bool active:1{}; // true - класс нормально функционирует. Disconnect выключит всё
    bool broadcast:1{}; // разрешён бродкаст
    bool server:1{}; // режим сервера
    bool has_packets:1{}; // доступны принятые пакеты
    uint received_packets {}; // сколько пакетов принято
    uint sended_packets {}; // сколько пакетов отправено
    Str ip_v4 {};
    net::Port port_tcp {};
    net::Port port_tcp {};
  };

  Packet_mgr();
  virtual ~Packet_mgr();
  void start(cr<Config> cfg);
  void disconnect();
  void update(); // это обязательно вызывать чтобы ассинорхонные отправки завершались
  void send(cr<Packet> src, cr<Target_info> target); // отправка пакета
  [[nodiscard]] Packets unload_all(); // выгрузить все скачанные пакеты
  [[nodiscard]] cr<Status> status() const;
private:
  struct Impl;
  Unique<Impl> _impl {};
};

} // net ns
