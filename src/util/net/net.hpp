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

constexpr static const std::size_t PACKET_BUFFER_SZ = 508;
constexpr static const Port DEFAULT_PORT = 49'099;
constexpr static const auto MY_IPV4 = "127.0.0.1";
constexpr static const u32_t SHORT_NICKNAME_SZ = 50;

enum class Tag: byte {
  EMPTY = 0,
  MESSAGE, // текстовое сообщение
  CONNECTION_INFO, // инфа о подключении и состоянии сети
  DISCONNECT, // сигнал выключающий сервер или клиента
  CONNECTED, // сервер или клиент смогли приконнектиться
};

struct Packet final {
  Port port {DEFAULT_PORT};
  Str ip_v4 {MY_IPV4};
  Bytes bytes {};
};

using Packets = Vector<Packet>;

} // net ns
