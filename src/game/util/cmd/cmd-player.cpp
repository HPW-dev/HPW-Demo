#include <cassert>
#include "cmd-player.hpp"
#include "cmd-util.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "game/core/entities.hpp"
#include "game/entity/player/player.hpp"
#include "game/entity/player/ability/ability-util.hpp"

void give_ability(Cmd_maker& command, Cmd& console, cr<Strs> args) {
  iferror(args.size() < 2, "нужно название способности");
  Str ability_name = args.at(1);
  uint ability_lvl = 1;
  if (args.size() > 2)
    ability_lvl = s2n<uint>(args.at(2));

  assert(hpw::entity_mgr);
  auto player = hpw::entity_mgr->get_player();
  iferror(!player, "нужен игрок");

  player->give(get_id(ability_name), ability_lvl);
  console.print("игроку назначена способность \"" + ability_name + "\" "
    + n2s(ability_lvl) + " уровня");
}

Strs give_ability_matches(Cmd_maker& command, Cmd& console, cr<Strs> args) {
  return {}; // TODO
}

void cmd_player_init(Cmd& cmd) {
  #define MAKE_CMD(NAME, DESC, EXEC_F, MATCH_F) \
    cmd.move( new_unique<Cmd_maker>(cmd, NAME, DESC, EXEC_F, \
      Cmd_maker::Func_command_matches{MATCH_F}) );

  MAKE_CMD("give", "give <ability_name> <level> - give ability for player", &give_ability, &give_ability_matches)
  // resurect
  // abilities
  #undef MAKE_CMD
} // cmd_core_init
