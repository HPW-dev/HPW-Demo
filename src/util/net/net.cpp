#include "net.hpp"

namespace net {

inline static void hash_step(Hash& dst, cp<byte> src, const std::size_t sz) {
  assert(sz != 0);
  assert(sz < 512 * 1024 * 1024);
  assert(src);

  constexpr Hash MUL = 255;
  constexpr Hash ADD = 1;
  cfor (i, sz) {
    dst *= MUL;
    dst += scast<Hash>(*src);
    ++src;
    dst += ADD;
  }
}

Hash get_hash(cr<Packet> src) {
  assert(!src.bytes.empty());
  // не учитывать сами данные контрольной суммы
  auto data_size = src.bytes.size();
  assert(data_size - sizeof(Hash) > 0);
  data_size -= sizeof(Hash);

  Hash ret {0xFFFFu};
  hash_step(ret, src.bytes.data(), data_size);
  return ret;
}

Hash find_packet_hash(cr<Packet> src) {
  assert(!src.bytes.empty());
  cauto data_size = src.bytes.size();
  assert(data_size - sizeof(Hash) > 0);
  cauto hash_addr = src.bytes.data() + data_size - sizeof(Hash);
  return *(cptr2ptr<cp<Hash>>(hash_addr));
}

} // net ns
