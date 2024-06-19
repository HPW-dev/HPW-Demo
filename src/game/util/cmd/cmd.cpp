#include "cmd.hpp"

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
