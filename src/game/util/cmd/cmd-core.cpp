#include "cmd-util.hpp"

void cmd_core_init(Cmd& cmd) {  
  cmd.move( new_unique<Cmd_helper>(cmd,
    "delme",
    "delme (rly)",
    [](Cmd_helper& command, Cmd& console, CN<Strs> args) {},
    [](Cmd_helper& command, Cmd& console, CN<Strs> args)->Strs { return {}; }
  ) );
  //move(new_unique<Cmd_fps>());
  //move(new_unique<Cmd_render>());
  //move(new_unique<Cmd_tickrate>());
  //move(new_unique<Cmd_stats>());
  //move(new_unique<Cmd_end_stats>());
}
