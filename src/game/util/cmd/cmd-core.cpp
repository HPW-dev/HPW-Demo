#include "cmd-util.hpp"

void set_fps_limit(Cmd_maker& command, Cmd& console,
CN<Strs> args) {
  // TODO
}

void enable_render(Cmd_maker& command, Cmd& console,
CN<Strs> args) {
  // TODO
}

void set_tickrate(Cmd_maker& command, Cmd& console,
CN<Strs> args) {
  // TODO
}

void start_stat_record(Cmd_maker& command, Cmd& console,
CN<Strs> args) {
  // TODO
}

void end_stat_record(Cmd_maker& command, Cmd& console,
CN<Strs> args) {
  // TODO
}

void cmd_core_init(Cmd& cmd) {
  #define MAKE_CMD(NAME, DESC, EXEC_F, MATCH_F) \
    cmd.move( new_unique<Cmd_maker>(cmd, NAME, DESC, EXEC_F, \
      Cmd_maker::Func_command_matches{MATCH_F}) );

  MAKE_CMD (
    "fps",
    "fps <limit> - set fps limit. If limit = 0, disable limit",
    &set_fps_limit, {} )
  MAKE_CMD (
    "render",
    "render <1/0> - enable/disable rendering",
    &enable_render, {} )
  MAKE_CMD (
    "tickrate",
    "tickrate <limit> - sets count of updates per. sec.",
    &set_tickrate, {} )
  MAKE_CMD (
    "stats_start",
    "stats_start <filename> <seconds> - save statistics to file",
    &start_stat_record, {} )
  MAKE_CMD (
    "stats_end",
    "stats_end - end of saving statistics to file",
    &end_stat_record, {} )
    
  #undef MAKE_CMD
} // cmd_core_init
