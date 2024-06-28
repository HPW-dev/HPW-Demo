#include "cmd-util.hpp"
#include "game/core/graphic.hpp"
#include "host/host-util.hpp"
#include "game/util/sync.hpp"
#include "game/util/config.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"

void set_fps_limit(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "не указано количество FPS в команде");
  cauto new_fps = s2n<int>(args[1]);

  if (new_fps <= 0) {
    graphic::set_disable_frame_limit(true);
    console.print("лимит FPS выключен");
  } else {
    graphic::set_disable_frame_limit(false);
    graphic::set_target_fps(new_fps);
    console.print("лимит FPS = " + n2s(new_fps));
  }
}

void config_reload(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  load_config(); 
  console.print("конфиг перезагружен");
}

void enable_render(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "у команды "
    + command.name() + " не задан параметр");
  const bool yesno = s2n<int>(args[1]) == 0 ? false : true;
  graphic::enable_render = yesno;
  console.print(Str("рендер ") + (yesno ? "включён" : "выключен"));
}

void set_tickrate(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "не указано количество UPS в команде");
  cauto new_ups = s2n<int>(args[1]);
  set_target_ups(new_ups);
  console.print("тикрейт игры = " + n2s(new_ups));
}

void start_stat_record(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 3, "в команде stat_record задано мало параметров");
  
}

void end_stat_record(Cmd_maker& command, Cmd& console, CN<Strs> args) {
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
  MAKE_CMD (
    "config_reload",
    "config_reload - reload config.yml",
    &config_reload, {} )
    
  #undef MAKE_CMD
} // cmd_core_init
