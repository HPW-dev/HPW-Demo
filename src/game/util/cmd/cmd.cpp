#include <cassert>
#include <utility>
#include "cmd.hpp"
#include "cmd-entity.hpp"
#include "cmd-phys.hpp"
#include "cmd-common.hpp"
#include "cmd-load.hpp"
#include "cmd-help.hpp"
#include "cmd-print.hpp"
#include "cmd-player.hpp"
#include "cmd-level.hpp"
#include "cmd-core.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "game/core/messages.hpp"

Cmd::Cmd() {
  // TODO
}

void Cmd::move(Unique<Command>&& command)
  { m_commands.emplace_back( std::move(command) ); }

void Cmd::exec(CN<Str> cmd_and_args) {
  try {
    cauto splited = split_str(cmd_and_args, ' ');
    cnauto command = find_command(splited.at(0));
    command->exec(splited);
  } catch (CN<hpw::Error> err) {
    print(U"error while execute command \"" +
      sconv<utf32>(cmd_and_args) + U"\":\n" + sconv<utf32>(err.what()));
  } catch (...) {
    print(U"undefined error while execute command \"" +
      sconv<utf32>(cmd_and_args) + U"\"");
  }
  m_last_cmd = cmd_and_args;
}

Strs Cmd::command_matches(CN<Str> cmd_and_args) const {
  return {}; // TODO
}

void Cmd::print(CN<utf32> text) const {
  if (m_log_screen)
    hpw_log(sconv<Str>(text) << '\n');

  if (m_log_console) {
    Message msg;
    msg.text = text;
    msg.lifetime = 3.5;
    hpw::message_mgr->move(std::move(msg));
  }
}

CN<Unique<Cmd::Command>> Cmd::find_command(CN<Str> name) const {
  cauto lower_name = str_tolower(name);
  cnauto finded_cmd = std::find_if(m_commands.begin(), m_commands.end(),
    [&](CN<Unique<Command>> cmd) { return cmd->name() == lower_name; });
  return_if (finded_cmd != m_commands.end(), *finded_cmd);
  error("not finded command \"" << name << "\"");

  // заглушка для анализатора
  static Unique<Command> error {};
  return error; 
}
