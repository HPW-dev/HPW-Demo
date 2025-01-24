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

#pragma pack(push, 1)
struct Packet final {
  Port port {DEFAULT_PORT};
  Str ip_v4 {MY_IPV4};
  Bytes bytes {};
};
#pragma pack(pop)

using Packets = Vector<Packet>;

template <class T>
inline T& bytes_to_packet(Bytes& src) {
  assert(src.size() == sizeof(T));
  return *(ptr2ptr<T*>(src.data()));
}

// получить контрольную сумму по данным пакета
Hash get_hash(cr<Packet> src);
// найти в данных пакета инфу о контрольной сумме
Hash find_packet_hash(cr<Packet> src);

} // net ns
