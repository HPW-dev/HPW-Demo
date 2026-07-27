#pragma once
#ifndef USE_NETPLAY
#pragma error "add define for USE_NETPLAY (-DUSE_NETPLAY)"
#endif
#include <cassert>
#include "util/bytes.hpp"
#include "util/str.hpp"
#include "util/macro.hpp"

namespace net {

using Hash = u32_t;
using Port = u16_t;

constexpr static const std::size_t MAX_UDP_PACKET = 508;
constexpr static const std::size_t MAX_TCP_PACKET = 8192;
constexpr static const Port AUTO_PORT = 0; // для автоматического назначения в UDP/TCP
constexpr static const Port SERVER_UDP_PORT = 49'091;
constexpr static const Port SERVER_TCP_PORT = 49'092;
constexpr static const Port CLIENT_UDP_PORT = AUTO_PORT;
constexpr static const Port CLIENT_TCP_PORT = AUTO_PORT;
constexpr static const auto SELF_IPV4 = "0.0.0.0";
constexpr static const u32_t SHORT_NICKNAME_SZ = 50;

enum class Tag: byte {
  EMPTY = 0,
  MESSAGE, // текстовое сообщение
  CONNECTION_INFO, // инфа о подключении и состоянии сети
  DISCONNECTED, // сигнал выключающий сервер или клиента
  CONNECTED, // сервер или клиент смогли приконнектиться
};

struct Packet final {
  Port port {}; // порт, по которому пришёл пакет
  Str ip_v4 {}; // ip v4, по которому пришёл пакет
  Bytes bytes {}; // данные для отправки или принятые
  bool by_udp {}; // true - пакет был отправлен по udp
};

using Packets = Vector<Packet>;

} // net ns
