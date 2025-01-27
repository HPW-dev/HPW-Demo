#include "packet.hpp"
#include "util/error.hpp"

namespace net {

void push_bytes(Bytes& dst, cp<byte> src, std::size_t src_sz) {
  // TODO
}

void read_bytes(cr<Bytes> src, byte* dst, std::size_t dst_sz, std::size_t& pos) {
  // TODO
}

void push_short_nickname(Bytes& dst, cr<utf32> nickname) {
  // TODO
}

void push_str(Bytes& dst, cr<Str> str) {
  // TODO
}

void push_utf32(Bytes& dst, cr<utf32> str) {
  // TODO
}

utf32 read_short_nickname(cr<Bytes> dst, std::size_t& pos) {
  return {}; // TODO
}

Str read_str(cr<Bytes> dst, std::size_t& pos) {
  return {}; // TODO
}

utf32 read_utf32(cr<Bytes> dst, std::size_t& pos) {
  return {}; // TODO
}

} // net ns
