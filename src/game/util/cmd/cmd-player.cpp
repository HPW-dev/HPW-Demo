#include <ranges>
#include <cassert>
#include "cmd-player.hpp"
#include "cmd-util.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/unicode.hpp"
#include "game/core/entities.hpp"
#include "game/util/vec-helper.hpp"
#include "game/entity/player/player.hpp"
#include "game/entity/player/ability/ability-util.hpp"

static Player* get_player() {
  assert(hpw::entity_mgr);
  auto player = hpw::entity_mgr->get_player();
  iferror(!player, "нужен игрок");
  return player;
}

void give_ability(Cmd_maker& command, Cmd& console, cr<Strs> args) {
  iferror(args.size() < 2, "нужно название способности");
  Str ability_name = args.at(1);
  uint ability_lvl = 1;
  if (args.size() > 2)
    ability_lvl = s2n<uint>(args.at(2));

  auto player = get_player();
  player->give(get_id(ability_name), ability_lvl);
  console.print("игроку назначена способность \"" + ability_name + "\" "
    + n2s(ability_lvl) + " уровня");
}

Strs give_ability_matches(Cmd_maker& command, Cmd& console, cr<Strs> args) {
  cauto cmd_name = args.at(0);
  Strs ret;
  cauto ability_names = get_ability_names();

  if (args.size() < 2) {
    // предложить названия способностей из списка
    for (crauto ability_name: ability_names)
      ret.push_back(cmd_name + ' ' + ability_name);
    return ret;
  }

  // по вводу определить что взять из списка автодополнения
  cauto arg_name = args.at(1); // имя способности введённое юзером
  cauto name_filter = [&](cr<Str> it) { return it.find(arg_name) == 0; };
  for (crauto ability_name: ability_names | std::views::filter(name_filter))
    ret.push_back(cmd_name + ' ' + ability_name);
  return ret;
}

void clear_abilities(Cmd_maker& command, Cmd& console, cr<Strs> args) {
  auto player = get_player();
  player->clear_abilities();
  console.print("все способности игрока удалены");
}

void print_abilities(Cmd_maker& command, Cmd& console, cr<Strs> args) {
  cauto player = get_player();
  cauto abilities = player->abilities();
  if (abilities.empty()) {
    console.print("у игрока нет способностей");
    return;
  }

  Str txt = "способности игрока:\n";
  for (cauto ability: abilities)
    txt += "- " + get_name(ability->id()) + " ур. " + n2s(ability->level()) + '\n';
  console.print(txt);
}

void resurect(Cmd_maker& command, Cmd& console, cr<Strs> args) {
  assert(hpw::entity_mgr);
  if (cauto player = hpw::entity_mgr->get_player(); player) {
    console.print("игрок и так живой");
  } else {
    hpw::entity_mgr->make({}, "player.boo.dark", get_screen_center());
    console.print("игрок воскрешён");
  }
}

void cmd_player_init(Cmd& cmd) {
  #define MAKE_CMD(NAME, DESC, EXEC_F, MATCH_F) \
    cmd.move( new_unique<Cmd_maker>(cmd, NAME, DESC, EXEC_F, \
      Cmd_maker::Func_command_matches{MATCH_F}) );

  MAKE_CMD("give", "give <ability_name> <level> - даёт игроку способность", &give_ability, &give_ability_matches)
  MAKE_CMD("clear_abilities", "убирает все способности у игрока", &clear_abilities, {})
  MAKE_CMD("abilities", "показать все способности, которые сейчас есть у игрока", &print_abilities, {})
  MAKE_CMD("resurect", "воскрешает игрока", &resurect, {})
  // resurect
  #undef MAKE_CMD
} // cmd_core_init
