#pragma once
#include <functional>
#include "cmd.hpp"

/* я задолбался расписывать наследников от базы, поэтому:
cmd.move( new_unique<Cmd_maker>(console,
  "delme",
  "delme (rly)",
  [](Cmd_maker& command, Cmd& console, cr<Strs> args) {},
  [](Cmd_maker& command, Cmd& console, cr<Strs> args)->Strs { return {}; }
) ); */
class Cmd_maker final: public Cmd::Command {
public:
  using Func_exec = std::function<void (Cmd_maker&, Cmd&, cr<Strs>)>;
  using Func_command_matches = std::function<Strs (Cmd_maker&, Cmd&, cr<Strs>)>;

  explicit Cmd_maker(Cmd& cmd, cr<Str> _name, cr<Str> _description,
    cr<Func_exec> _exec, cr<Func_command_matches> _command_matches={});
  ~Cmd_maker() = default;
  Str name() const override;
  Str description() const override;
  void exec(cr<Strs> cmd_and_args) override;
  Strs command_matches(cr<Strs> cmd_and_args) override;

private:
  Cmd& m_cmd;
  Str m_name {}; // имя команды
  Str m_description {}; // описание использования
  Func_exec m_exec {};
  Func_command_matches m_command_matches {};
}; // Cmd_maker
