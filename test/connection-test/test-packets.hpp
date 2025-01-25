#pragma once
#include <memory>
#include "util/net/net.hpp"

constexpr static const uint SHORT_NICKNAME_SZ = 50;

enum class Tag: byte {
  EMPTY = 0,
  ERROR, // заставляет кинуть hpw::error
  MESSAGE, // текстовое сообщение
  CLIENT_CONNECT, // запрос на подключение к серверу
  SERVER_BROADCAST, // широковещательное приглашение от сервера
};

#pragma pack(push, 1)
struct Version {
  byte major {};
  u16_t minor {};
  u16_t feature {};
  byte patch {};

  bool operator ==(cr<Version> other) const {
    return
      major == other.major &&
      minor == other.minor &&
      feature == other.feature &&
      patch == other.patch
    ;
  }
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

template <class T>
net::Packet new_packet() {
  net::Packet dst {};
  dst.bytes.resize(sizeof(T));
  cauto ret = new (dst.bytes.data()) T();
  assert(ret);
  return dst;
}

void prepare_game_version(Version& dst);
void prepare_short_nickname(char32_t short_nickname[], const uint sz);

// найти в данных пакета метку пакета
Tag find_packet_tag(cr<net::Packet> src);
