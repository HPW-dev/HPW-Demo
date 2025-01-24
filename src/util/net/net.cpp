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
  Hash ret {0xFFFFu};
  hash_step(ret, cptr2ptr<cp<byte>>(&src.tag), sizeof(src.tag));
  hash_step(ret, src.bytes.data(), src.bytes.size());
  return ret;
}

} // net ns
