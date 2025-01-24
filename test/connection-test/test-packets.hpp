#pragma once
#include "util/math/num-types.hpp"

constexpr static const uint SHORT_NICKNAME_SZ = 30;

#pragma pack(push, 1)
struct Version {
  byte version_major {};
  u16_t version_minor {};
  u16_t version_maintance {};
  byte version_patch {};
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Packet_broadcast {
  char32_t short_nickname[SHORT_NICKNAME_SZ]; // сокращённый ник сервера
  u16_t connected_players {}; // сколько игроков уже подключено к серверу
  Version game_version {}; // с какой версией игры играет сервак
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Packet_connect {
  char32_t short_nickname[SHORT_NICKNAME_SZ]; // сокращённый ник клиента
  Version game_version {}; // с какой версией игры играет клиент
};
#pragma pack(pop)

void prepare_game_version(Version& dst);
void prepare_short_nickname(char32_t* short_nickname, const uint sz);
