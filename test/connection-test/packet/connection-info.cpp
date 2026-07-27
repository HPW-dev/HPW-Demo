#include "connection-info.hpp"
#include "util/error.hpp"

namespace net {

void Pck_connection_info::from_packet(cr<Packet> src) {
  std::size_t pos {};

  cauto tag = read_data<Tag>(src.bytes, pos);
  iferror(tag != this->tag(), "теги пакета не совпадают");

  this->is_server = read_data<bool>(src.bytes, pos);
  this->self_nickname = read_short_nickname(src.bytes, pos);

  const u32_t player_count = read_data<u32_t>(src.bytes, pos);
  assert(player_count < 2'000);
  cfor (i, player_count) {
    Player_info player;
    player.nickname = read_short_nickname(src.bytes, pos);
    player.ip_v4 = read_str(src.bytes, pos);
    player.connected = read_data<bool>(src.bytes, pos);
    players.emplace_back(std::move(player));
  }

  cauto hash = read_data<Hash>(src.bytes, pos);
  cauto local_hash = get_packet_hash(to_packet(false));
  iferror(hash != local_hash, "чексумма пакета не совпадает");
}

Packet Pck_connection_info::to_packet(bool with_hash) const {
  Packet ret;
  push_data(ret.bytes, this->tag());
  push_data(ret.bytes, is_server);
  push_short_nickname(ret.bytes, self_nickname);

  push_data(ret.bytes, scast<u32_t>(players.size()));
  for (crauto player: players) {
    push_short_nickname(ret.bytes, player.nickname);
    push_str(ret.bytes, player.ip_v4);
    push_data(ret.bytes, player.connected);
  }
  
  if (with_hash)
    push_data(ret.bytes, get_packet_hash(ret));
  return ret;
}

} // net ns
