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
  // выполнить команду
  void exec(CN<Str> command);
  // получить прыдыдущую команду
  Str last_command() const;
}; // Cmd

namespace hpw {
inline Unique<Cmd> cmd {};
}
