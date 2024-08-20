#include "cmd-player.hpp"
#include "cmd-util.hpp"
#include "util/error.hpp"

void give_ability(Cmd_maker& command, Cmd& console, cr<Strs> args) {
  // TODO
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
