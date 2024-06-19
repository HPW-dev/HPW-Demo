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
#include "game/core/messages.hpp"

Cmd::Cmd() {
  // TODO
}

void Cmd::move(Unique<Command>&& command) {
  // TODO
}

void Cmd::exec(CN<Str> cmd_and_args) {
  // TODO
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
