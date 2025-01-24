#pragma once
#ifndef USE_NETPLAY
#pragma error "add define for USE_NETPLAY (-DUSE_NETPLAY)"
#endif
#include "util/bytes.hpp"
#include "util/str.hpp"

namespace net {
  
struct Packet final {
  Str source_address {}; // от кого пришёл пакет
  Bytes bytes {}; // данные пакета
};

using Packets = Vector<Packet>;

constexpr static const std::size_t PACKET_BUFFER_SZ = 400;
constexpr static const u16_t DEFAULT_PORT = 49'099;

} // net ns
