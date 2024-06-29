#include <cassert>
#include "cmd-entity.hpp"
#include "cmd-util.hpp"
#include "game/core/entities.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"

void cmd_entity_init(Cmd& cmd) {
  #define MAKE_CMD(NAME, DESC, EXEC_F, MATCH_F) \
    cmd.move( new_unique<Cmd_maker>(cmd, NAME, DESC, EXEC_F, \
      Cmd_maker::Func_command_matches{MATCH_F}) );

  /*MAKE_CMD (
    "fps",
    "fps <limit> - set fps limit. If limit = 0, disable limit",
    &set_fps_limit, {} )
  MAKE_CMD (
    "render",
    "render <1/0> - enable/disable rendering",
    &enable_render, {} )*/
    
  #undef MAKE_CMD
} // cmd_entity_init
