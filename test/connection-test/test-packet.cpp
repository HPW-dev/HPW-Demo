#include <cassert>
#include "test-packets.hpp"

void prepare_game_version(Version& dst) {
  // TODO
}

void prepare_short_nickname(char32_t* short_nickname, const uint sz) {
  assert(short_nickname);
  assert(sz > 0);
  assert(sz < 512 * 1024 * 1024);

  // TODO
}
