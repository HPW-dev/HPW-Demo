#pragma once
#include <optional>
#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"
#include "util/vector-types.hpp"
#include "game/util/keybits.hpp"
#include "game/core/difficulty.hpp"

using Key_packet = Vector<hpw::keycode>;

// Game replay (keylogger)
class Replay final {
public:
  struct Info;

  explicit Replay(cr<Str> path, bool write_mode);
  ~Replay();
  void close();
  void push(cr<Key_packet> key_packet);
  std::optional<Key_packet> pop(); // будет возвращать нажатые клавиши, пока не кончатся
  static Info get_info(cr<Str> path);
  utf32 warnings() const; // посмотреть проблемы с реплеем, если они есть

private:
  nocopy(Replay);
  struct Impl;
  Unique<Impl> impl {};
};

struct Date {
  uint year {};
  uint month {};
  uint day {};
  uint hour {};
  uint minute {};
  uint second {};
};

struct Replay::Info {
  Date date {};
  utf32 player_name {};
  Str path {};
  Str date_str {};
  Str hud_name {};
  std::int64_t score {};
  Difficulty difficulty {};
  bool first_level_is_tutorial {};
};

Str get_random_replay_name();
