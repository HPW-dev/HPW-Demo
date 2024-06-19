#include <cassert>
#include <utility>
#include <ranges>
#include <algorithm>
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
  //move(new_unique<Dummy>(this));
}

void Cmd::move(Unique<Command>&& command) { 
  assert(command);
  m_commands.emplace_back( std::move(command) );
}

void Cmd::exec(CN<Str> cmd_and_args) {
  try {
    cauto splited = split_str(cmd_and_args, ' ');
    cnauto command = find_command(splited.at(0));
    iferror(!command, "not finded command \"" << cmd_and_args << "\"");
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

Strs Cmd::command_matches(CN<Str> cmd_and_args) {
  Strs ret;

  // если нет параметров, показать все команды и выйти
  if (cmd_and_args.empty()) {
    for (cnauto command: m_commands)
      ret.push_back(command->name());
    return ret;
  }

  cauto args = split(cmd_and_args, ' ');
  cauto cmd_name = str_tolower( args.at(0) );

  // учитывать совпадения в начале слова
  auto command_match = [&](CN<decltype(m_commands)::value_type> command)
    { return command->name().find(cmd_name) == 0; };
  // найти совпадающие команды по их названию
  for (cnauto founded: m_commands | std::views::filter(command_match))
    ret.push_back(founded->name());
  // если команда введена верно
  auto cmd = find_command(cmd_name);
  return_if(!cmd, ret);
  // узнать, есть ли у команды свои предложения к дополнению команды
  cauto cmd_matches = cmd->command_matches(args);
  return_if(cmd_matches.empty(), ret);
  return cmd_matches;
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

Cmd::Command* Cmd::find_command(CN<Str> name) {
  cauto lower_name = str_tolower(name);
  auto finded_cmd = std::find_if(m_commands.begin(), m_commands.end(),
    [&](CN<Unique<Command>> cmd) { return cmd->name() == lower_name; });
  if (finded_cmd != m_commands.end()) {
    nauto ret = *finded_cmd;
    assert(ret);
    return ret.get();
  }

  return {};
}
