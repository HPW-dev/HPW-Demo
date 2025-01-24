#include <cassert>
#include <cstring>
#include <algorithm>
#include "test-packets.hpp"
#include "game/util/version.hpp"
#include "game/core/user.hpp"
#include "util/str-util.hpp"
#include "util/log.hpp"

void prepare_game_version(Version& dst) {
  Str ver = get_game_version();
  if (ver.empty()) {
    dst.major = dst.minor = dst.feature = dst.patch = '?';
  } else {
    cauto splited = split_str(ver, '.');
    try {
      dst.major   = s2n<int>(splited.at(0));
      dst.minor   = s2n<int>(splited.at(1));
      dst.feature = s2n<int>(splited.at(2));
      dst.patch   = s2n<int>(splited.at(3));
    } catch (...) {
      hpw_debug("ошибка при получении версии игры\n");
    }
  }
}

void prepare_short_nickname(char32_t short_nickname[], const uint sz) {
  assert(short_nickname);
  assert(sz > 0);
  assert(sz < 512 * 1024 * 1024);

  memset(ptr2ptr<char*>(short_nickname), '\0', sz * sizeof(char32_t));
  return_if(hpw::player_name.empty());

  cauto nick_sz = std::min<uint>(hpw::player_name.size(), sz);
  cfor (i, nick_sz)
    short_nickname[i] = hpw::player_name[i];
}
