#pragma once
#include <utility>
#include "util/mem-types.hpp"
#include "util/math/vec.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"
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
    virtual Str description() const = 0;
    // выполнить команду
    virtual void exec(cr<Strs> cmd_and_args) = 0;
    // повлиять на автодополнение
    inline virtual Strs command_matches(cr<Strs> cmd_and_args) { return {}; }
  };
  using Commands = Vector<Unique<Command>>;

  Cmd();
  // выполнить команду
  void exec(cr<Str> cmd_and_args);
  // получить прыдыдущую команду
  inline cr<Str> last_command() const { return m_last_cmd; }
  // найти совпадающие имена команд (для автодополнения)
  Strs command_matches(cr<Str> cmd_and_args);
  // получить все загруженные команды
  inline cr<Commands> commands() const { return m_commands; }
  // загрузить новую команду
  void move(Unique<Command>&& command);
  // разрешить выводить лог на игровой кран
  inline void enable_log_screen(const bool yesno) { m_log_screen = yesno; }
  // разрешить выводить лог в консоль системы
  inline void enable_log_console(const bool yesno) { m_log_console = yesno; }
  // печатает текст на экране игры и в консоль
  void print(cr<Str> text, bool is_error=false) const;
  inline Vec last_pos() const { return m_last_pos; }
  inline Uid last_uid() const { return m_last_uid; }

private:
  nocopy(Cmd);
  Commands m_commands {};
  Str m_last_cmd {}; // предыдущая команда
  bool m_log_screen {true};
  bool m_log_console {true};
  Uid m_last_uid {}; // последний uid entity, с которым взаимодействовали
  Vec m_last_pos {}; // последние корды, с которыми взаимодействовали

  Command* find_command(cr<Str> name);
  void impl_exec(cr<Str> cmd_and_args);
  // показать имена всех команд
  Strs command_names() const;
  // ищет совпадения в названиях команд
  Strs find_cmd_name_matches(cr<Str> cmd_name) const;
  void print_to_console(cr<Str> text, bool is_error=false) const;
  void print_to_screen(cr<Str> text) const;
  void sort_commands();
}; // Cmd

namespace hpw {
inline Cmd cmd {}; // исполняет команды в игре
}
