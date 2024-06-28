#pragma once
#include <functional>
#include "cmd.hpp"

/* я задолбался расписывать наследников от базы, поэтому:
cmd.move( new_unique<Cmd_helper>(console,
  "delme",
  "delme (rly)",
  [](Cmd_helper& command, Cmd& console, CN<Strs> args) {},
  [](Cmd_helper& command, Cmd& console, CN<Strs> args)->Strs { return {}; }
) ); */
class Cmd_helper final: public Cmd::Command {
public:
  using Func_exec = std::function<void (Cmd_helper&, Cmd&, CN<Strs>)>;
  using Func_command_matches = std::function<Strs (Cmd_helper&, Cmd&, CN<Strs>)>;

  explicit Cmd_helper(Cmd& cmd, CN<Str> _name, CN<Str> _description,
    CN<Func_exec> _exec, CN<Func_command_matches> _command_matches={});
  ~Cmd_helper() = default;
  Str name() const override;
  Str description() const override;
  void exec(CN<Strs> cmd_and_args) override;
  Strs command_matches(CN<Strs> cmd_and_args) override;

private:
  Cmd& m_cmd;
  Str m_name {}; // имя команды
  Str m_description {}; // описание использования
  Func_exec m_exec {};
  Func_command_matches m_command_matches {};
};
