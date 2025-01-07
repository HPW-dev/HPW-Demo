#pragma once
#include "util/bytes.hpp"
#include "util/str.hpp"

namespace net {
  
struct Packet {
  Str ip {};
  Bytes bytes {};
};

using Packets = Vector<Packet>;

} // net ns
