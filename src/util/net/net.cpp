#include "net.hpp"
#include "util/error.hpp"

namespace net {

Hash gen_hash(cp<byte> src, const std::size_t src_sz, Hash prev) {
  ret_if (!src, prev);
  ret_if (src_sz == 0, prev);
  assert(src_sz < 512 * 1024 * 1024);

  constexpr Hash MUL = 255;
  constexpr Hash ADD = 1;
  cfor (i, src_sz) {
    prev *= MUL;
    prev += scast<Hash>(*src);
    ++src;
    prev += ADD;
  }
  
  return prev;
}

Hash gen_packet_hash(cr<Packet> src) {
  // учесть тег пакета
  Hash ret = gen_hash(cptr2ptr<cp<byte>>(&src.tag), sizeof(src.tag));
  // учесть данные пакета
  ret = gen_hash(src.bytes.data(), src.bytes.size(), ret);
  return ret;
}

} // net ns
