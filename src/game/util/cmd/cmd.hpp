#pragma once
#include "util/mem-types.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"
#include "util/unicode.hpp"
#include "util/vector-types.hpp"

// исполняет текстовые команды
class Cmd final {
public:
  // База для команд
  class Command {
  public:
    virtual ~Command() = default;
    // название команды
    virtual Str name() const = 0;
    // описание команды
    virtual utf32 description() const = 0;
    // выполнить команду
    virtual void exec(CN<Strs> cmd_and_args) = 0;
    // повлиять на автодополнение
    inline virtual Strs command_matches(CN<Strs> cmd_and_args) { return {}; }
  };
  using Commands = Vector<Unique<Command>>;

  Cmd();
  // выполнить команду
  void exec(CN<Str> cmd_and_args);
  // получить прыдыдущую команду
  inline Str last_command() const { return m_last_cmd; }
  // найти совпадающие имена команд (для автодополнения)
  Strs command_matches(CN<Str> cmd_and_args);
  // получить все загруженные команды
  inline CN<Commands> commands() const { return m_commands; }
  // загрузить новую команду
  void move(Unique<Command>&& command);
  // разрешить выводить лог на игровой кран
  inline void enable_log_screen(const bool yesno) { m_log_screen = yesno; }
  // разрешить выводить лог в консоль системы
  inline void enable_log_console(const bool yesno) { m_log_console = yesno; }
  // печатает текст на экране игры и в консоль
  void print(CN<utf32> text) const;

private:
  Commands m_commands {};
  Str m_last_cmd {}; // предыдущая команда
  bool m_log_screen {true};
  bool m_log_console {true};

  Command* find_command(CN<Str> name);
}; // Cmd

namespace hpw {
inline Cmd cmd {}; // исполняет команды в игре
}
