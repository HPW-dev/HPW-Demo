#include <cassert>
#include "cmd-util.hpp"

Cmd_maker::Cmd_maker(Cmd& cmd, CN<Str> _name, CN<Str> _description,
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

Str Cmd_maker::name() const { return m_name; }
Str Cmd_maker::description() const { return m_description; }

void Cmd_maker::exec(CN<Strs> cmd_and_args) {
  if (m_exec)
    m_exec(*this, m_cmd, cmd_and_args);
}

Strs Cmd_maker::command_matches(CN<Strs> cmd_and_args) {
  if (m_command_matches)
    return m_command_matches(*this, m_cmd, cmd_and_args);
  return {};
}
