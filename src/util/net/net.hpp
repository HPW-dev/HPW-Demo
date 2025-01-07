#pragma once
#include <atomic>
#include "util/bytes.hpp"
#include "util/str.hpp"

namespace net {
  
struct Packet {
  Str source_address {}; // от кого пришёл пакет
  Bytes bytes {}; // данные пакета
  std::atomic_bool loaded_correctly {}; // true когда успешно получен
};

using Packets = Vector<Packet>;

constexpr static const std::size_t PACKET_BUFFER_SZ = 400;

} // net ns
