#include <cassert>
#include "cmd-util.hpp"

Cmd_helper::Cmd_helper(Cmd& cmd, CN<Str> _name, CN<Str> _description,
CN<Func_exec> _exec, CN<Func_command_matches> _command_matches)
: m_cmd {cmd}
, m_name {_name}
, m_description {_description}
, m_exec {_exec}
, m_command_matches {_command_matches}
{
  assert(!m_name.empty());
  assert(m_exec);
}

Str Cmd_helper::name() const { return m_name; }
Str Cmd_helper::description() const { return m_description; }

void Cmd_helper::exec(CN<Strs> cmd_and_args) {
  if (m_exec)
    m_exec(*this, m_cmd, cmd_and_args);
}

Strs Cmd_helper::command_matches(CN<Strs> cmd_and_args) {
  if (m_command_matches)
    return m_command_matches(*this, m_cmd, cmd_and_args);
  return {};
}
