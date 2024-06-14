#pragma once
#include "util/mem-types.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"

// исполняет текстовые команды
class Cmd {
  struct Impl;
  Unique<Impl> impl {};

public:
  Cmd();
  ~Cmd();
  void exec(CN<Str> command);
}; // Cmd

namespace hpw {
inline Unique<Cmd> cmd {};
}
