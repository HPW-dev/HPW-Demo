#pragma once
#include "util/bytes.hpp"
#include "util/str.hpp"

namespace net {
  
struct Packet {
  Str ip {};
  Bytes bytes {};
};

using Packets = Vector<Packet>;

constexpr static const std::size_t PACKET_BUFFER_SZ = 400;

} // net ns
