#pragma once
#include "util/bytes.hpp"
#include "util/str.hpp"

namespace net {
  
struct Packet final {
  Str source_address {}; // от кого пришёл пакет
  Bytes bytes {}; // данные пакета
};

using Packets = Vector<Packet>;

constexpr static const std::size_t PACKET_BUFFER_SZ = 400;

} // net ns
