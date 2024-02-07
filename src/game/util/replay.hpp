#pragma once
#include <optional>
#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"
#include "util/vector-types.hpp"
#include "game/util/keybits.hpp"

using Key_packet = Vector<hpw::keycode>;

/// Game replay (keylogger)
class Replay final {
  nocopy(Replay);
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Replay(CN<Str> path, bool write_mode);
  ~Replay();
  void close();
  void push(CN<Key_packet> key_packet);
  std::optional<Key_packet> pop(); /// будет возвращать нажатые клавиши, пока не кончатся
  CP<Impl> get_impl() const;
}; // Replay
