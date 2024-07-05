#include <cassert>
#include <utility>
#include <ranges>
#include <algorithm>
#include "cmd.hpp"
#include "cmd-entity.hpp"
#include "cmd-common.hpp"
#include "cmd-script.hpp"
#include "cmd-player.hpp"
#include "cmd-level.hpp"
#include "cmd-core.hpp"
#include "util/log.hpp"
#include "util/unicode.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "game/core/messages.hpp"

Cmd::Cmd() {
  //cmd-common.hpp
  move(new_unique<Cmd_exit>());
  move(new_unique<Cmd_error>());
  move(new_unique<Cmd_print>(this));
  move(new_unique<Cmd_alias>(this));
  move(new_unique<Cmd_log_cnosole>(this));
  move(new_unique<Cmd_log_screen>(this));
  move(new_unique<Cmd_help>(this));
  move(new_unique<Cmd_cls>());
  move(new_unique<Cmd_comment>());

  //cmd-script.hpp
  move(new_unique<Cmd_script>());

  //cmd-level.hpp
  move(new_unique<Cmd_levels>(this));
  move(new_unique<Cmd_level>(this));
  move(new_unique<Cmd_restart>(this));
  move(new_unique<Cmd_collider>(this));

  cmd_core_init(*this); // cmd-core.hpp
  cmd_entity_init(*this); // cmd-entity.hpp

  //cmd-entity.hpp
  //move(new_unique<Cmd_entities>());
  //move(new_unique<Cmd_spawn>());
  //move(new_unique<Cmd_kill>());
  //move(new_unique<Cmd_flag>());
  //move(new_unique<Cmd_hp>());
  //move(new_unique<Cmd_copy>());

  //cmd-phys.hpp
  //move(new_unique<Cmd_pos>());
  //move(new_unique<Cmd_deg>());
  //move(new_unique<Cmd_speed>());
  //move(new_unique<Cmd_force>());
  //move(new_unique<Cmd_direct>());
  //move(new_unique<Cmd_direct_to>());

  //cmd-player.hpp
  //move(new_unique<Cmd_resurect>());
  //move(new_unique<Cmd_abilities>());
  //move(new_unique<Cmd_give_ability>());

  sort_commands();
} // Cmd c-tor

void Cmd::sort_commands() { 
  // сортировка команд по имени
  cauto name_sorter = [](CN<Unique<Command>> a, CN<Unique<Command>> b)->bool
    { return a->name() < b->name(); };
  std::sort(m_commands.begin(), m_commands.end(), name_sorter);
}

void Cmd::move(Unique<Command>&& command) { 
  assert(command);
  m_commands.emplace_back( std::move(command) );
}

/* Разбивает строчки разделённые пробелами с учётом кавычек.
Кавычки в результат не входят */
inline Strs split_args_str(CN<Str> cmd_and_args) {
  constexpr const char separator = ' ';
  constexpr const char comma = '"';
  bool comma_mode = false;
  Str cur_str;
  Strs ret;

  for (cnauto ch: cmd_and_args) {
    if (ch == comma) {
      comma_mode = !comma_mode;
      continue;
    }

    if (comma_mode) {
      cur_str += ch;
      continue;
    }

    if (ch == separator) {
      if (!cur_str.empty())
        ret.push_back(cur_str);
      cur_str.clear();
      continue;
    }

    cur_str += ch;
  } // for all chars in cmd_and_args

  if (!cur_str.empty())
    ret.push_back(cur_str);

  return ret;
} // split_args_str

void Cmd::impl_exec(CN<Str> cmd_and_args) {
  cauto splited = split_args_str(cmd_and_args);
  cnauto command = find_command(splited.at(0));
  iferror(!command, "not finded command \"" << cmd_and_args << "\"");
  command->exec(splited);
}

void Cmd::exec(CN<Str> cmd_and_args) {
  return_if(cmd_and_args.empty());

  try {
    impl_exec(cmd_and_args);
  } catch (CN<hpw::Error> err) {
    print("error while execute command \"" +
      cmd_and_args + "\":\n" + err.what());
  } catch (...) {
    print("undefined error while execute command \"" +
      cmd_and_args + "\"");
  }
  
  m_last_cmd = cmd_and_args;
}

Strs Cmd::command_matches(CN<Str> cmd_and_args) {
  // если нет параметров, показать все команды и выйти
  return_if (cmd_and_args.empty(), command_names());

  cauto args = split(cmd_and_args, ' ');
  cauto cmd_name = str_tolower( args.at(0) );
  auto ret = find_cmd_name_matches(cmd_name);
  for (nauto cmd_name: ret)
    cmd_name += ' ';

  // если команда введена не до конца, то показать только совпадения
  auto cmd = find_command(cmd_name);
  return_if(!cmd, ret);

  // узнать, есть ли у команды свои предложения к дополнению команды
  cauto cmd_matches = cmd->command_matches(args);
  return_if(cmd_matches.empty(), ret);
  return cmd_matches;
} // command_matches

void Cmd::print_to_console(CN<Str> text) const {
  return_if(!m_log_console);
  hpw_log(sconv<Str>(text) << '\n');
}

void Cmd::print_to_screen(CN<Str> text) const {
  return_if(!m_log_screen);
  Message msg;
  msg.text = utf8_to_32(text);
  msg.lifetime = 3.5;
  assert(hpw::message_mgr);
  hpw::message_mgr->move(std::move(msg));
}

void Cmd::print(CN<Str> text) const {
  print_to_console(text);
  print_to_screen(text);
}

Cmd::Command* Cmd::find_command(CN<Str> name) {
  cauto lower_name = str_tolower(name);
  cauto finded_cmd = std::find_if(m_commands.begin(), m_commands.end(),
    [&](CN<Unique<Command>> cmd) { return cmd->name() == lower_name; });
  return_if (finded_cmd == m_commands.end(), nullptr);
  nauto ret = *finded_cmd;
  assert(ret);
  return ret.get();
}

Strs Cmd::command_names() const {
  Strs ret;
  for (cnauto command: m_commands)
    ret.push_back(command->name());
  return ret;
}

Strs Cmd::find_cmd_name_matches(CN<Str> cmd_name) const {
  // учитывать совпадения в начале слова
  auto cmd_name_filter = [&](CN<decltype(m_commands)::value_type> command)
    { return command->name().find(cmd_name) == 0; };
  // найти совпадающие команды по их названию
  Strs ret;
  for (cnauto founded: m_commands | std::views::filter(cmd_name_filter))
    ret.push_back(founded->name());
  return ret;
}
