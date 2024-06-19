#pragma once
#include "util/mem-types.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"
#include "util/unicode.hpp"
#include "util/vector-types.hpp"

// исполняет текстовые команды
class Cmd final {
  // База для команд
  class Command {
  public:
    Str name {}; // название команды
    utf32 description {};
    
    virtual ~Command() = default;
    virtual void exec(CN<Strs> cmd_and_args) = 0;
    // повлиять на автодополнение
    inline virtual Strs add_matches(CN<Strs> cmd_and_args) const { return {}; }
  };

  using Commands = Vector<Unique<Command>>;
  Commands m_commands {};
  Str m_last_cmd {}; // предыдущая команда

public:
  Cmd();
  // выполнить команду
  void exec(CN<Str> cmd_and_args);
  // получить прыдыдущую команду
  inline Str last_command() const { return m_last_cmd; }
  // найти совпадающие имена команд (для автодополнения)
  Strs command_matches(CN<Str> cmd_and_args) const;
  // получить все загруженные команды
  inline CN<Commands> commands() const { return m_commands; }
  // загрузить новую команду
  void move(Unique<Command>&& command);
}; // Cmd

namespace hpw {
inline Cmd cmd {}; // исполняет команды в игре
}
