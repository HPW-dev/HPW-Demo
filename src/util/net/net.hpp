#pragma once
#ifndef USE_NETPLAY
#pragma error "add define for USE_NETPLAY (-DUSE_NETPLAY)"
#endif
#include "util/bytes.hpp"
#include "util/str.hpp"

namespace net {

using Hash = u32_t;

struct Packet final {
  enum class Tag: byte {
    EMPTY = 0,
    ERROR, // заставляет кинуть hpw::error
    MESSAGE, // текстовое сообщение
    CONNECT, // запрос на подключение к серверу
    SERVER_BROADCAST, // широковещательное приглашение от сервера
  };

  Str source_address {}; // от кого пришёл пакет
  Tag tag {}; // тип пакета
  Bytes bytes {}; // данные пакета
  Hash hash {}; // контрольная сумма пакета
};

using Packets = Vector<Packet>;

constexpr static const std::size_t PACKET_BUFFER_SZ = 400;
constexpr static const u16_t DEFAULT_PORT = 49'099;
constexpr static const auto MY_IPV4 = "127.0.0.1";

} // net ns
