#include "connection-info.hpp"

namespace net {

void Pck_connection_info::from_packet(cr<Packet> src) {
  std::size_t pos {};
  is_server = read_data<bool>(src.bytes, pos);
  self_nickname = read_short_nickname(src.bytes, pos);

  const u32_t player_count = read_data<u32_t>(src.bytes, pos);
  assert(player_count < 2'000);
  cfor (i, player_count) {
    Player_info player;
    player.nickname = read_short_nickname(src.bytes, pos);
    player.ip_v4 = read_str(src.bytes, pos);
    player.connected = read_data<bool>(src.bytes, pos);
    players.emplace_back(std::move(player));
  }
}

Packet Pck_connection_info::to_packet() const {
  Packet ret;
  ret.tag = net::Tag::CONNECTION_INFO;
  push_data(ret.bytes, is_server);
  push_short_nickname(ret.bytes, self_nickname);

  push_data(ret.bytes, scast<u32_t>(players.size()));
  for (crauto player: players) {
    push_short_nickname(ret.bytes, player.nickname);
    push_str(ret.bytes, player.ip_v4);
    push_data(ret.bytes, player.connected);
  }

  return ret;
}

} // net ns
