#include "disconnected.hpp"
#include "util/error.hpp"

namespace net {

void Pck_disconnected::from_packet(cr<Packet> src) {
  std::size_t pos {};

  cauto tag = read_data<Tag>(src.bytes, pos);
  iferror(tag != this->tag(), "теги пакета не совпадают");

  disconnect_you = read_data<bool>(src.bytes, pos);

  cauto hash = read_data<Hash>(src.bytes, pos);
  cauto local_hash = get_packet_hash(to_packet(false));
  iferror(hash != local_hash, "чексумма пакета не совпадает");
}

Packet Pck_disconnected::to_packet(bool with_hash) const {
  Packet ret;
  push_data(ret.bytes, this->tag());
  push_data(ret.bytes, disconnect_you);

  if (with_hash)
    push_data(ret.bytes, get_packet_hash(ret));
    
  return ret;
}

} // net ns
