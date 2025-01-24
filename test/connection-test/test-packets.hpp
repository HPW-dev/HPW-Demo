#pragma once
#include "util/net/net.hpp"

constexpr static const uint SHORT_NICKNAME_SZ = 30;

enum class Tag: byte {
  EMPTY = 0,
  ERROR, // заставляет кинуть hpw::error
  MESSAGE, // текстовое сообщение
  CLIENT_CONNECT, // запрос на подключение к серверу
  SERVER_BROADCAST, // широковещательное приглашение от сервера
};

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
  Tag tag {Tag::SERVER_BROADCAST};
  char32_t short_nickname[SHORT_NICKNAME_SZ]; // сокращённый ник сервера
  u16_t connected_players {}; // сколько игроков уже подключено к серверу
  Version game_version {}; // с какой версией игры играет сервак
  net::Hash hash {}; // контрольная сумма пакета
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Packet_connect {
  Tag tag {Tag::CLIENT_CONNECT};
  char32_t short_nickname[SHORT_NICKNAME_SZ]; // сокращённый ник клиента
  Version game_version {}; // с какой версией игры играет клиент
  net::Hash hash {}; // контрольная сумма пакета
};
#pragma pack(pop)

void prepare_game_version(Version& dst);
void prepare_short_nickname(char32_t* short_nickname, const uint sz);
