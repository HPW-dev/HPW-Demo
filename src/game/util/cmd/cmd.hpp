#pragma once
#include "util/mem-types.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"
#include "util/unicode.hpp"
#include "util/vector-types.hpp"

// База для команд
class Command {
public:
  Str name {}; // название команды
  utf32 description {};
  virtual ~Command() = default;
  virtual void exec(CN<Strs> cmd_and_args);
  // повлиять на автодополнение
  inline virtual Strs add_matches(CN<Strs> cmd_and_args) const { return {}; }
};

using Commands = Vector<Unique<Command>>;

// исполняет текстовые команды
class Cmd {
  struct Impl;
  Unique<Impl> impl {};

public:
  Cmd();
  ~Cmd();
  // выполнить команду
  void exec(CN<Str> cmd_and_args);
  // получить прыдыдущую команду
  Str last_command() const;
  // найти совпадающие имена команд (для автодополнения)
  Strs command_matches(CN<Str> cmd_and_args) const;
  // получить все загруженные команды
  CN<Commands> commands() const;
  // загрузить новую команду
  void move(Unique<Command>&& command);
}; // Cmd

namespace hpw {
inline Unique<Cmd> cmd {}; // исполняет команды в игре
}
