#include "cmd.hpp"

struct Cmd::Impl {
  Commands m_commands {};
  Str m_last_cmd {}; // предыдущая команда

  inline Impl() {
    // init commands:

  } // c-tor

  inline void move(Unique<Command>&& command) {

  }

  inline void exec(CN<Str> cmd_and_args) {

  }

  inline Strs command_matches(CN<Str> cmd_and_args) {
    return {};
  }

  inline Str last_command() const { return m_last_cmd; }
  inline CN<Commands> commands() const { return m_commands; }
}; // Impl

Cmd::Cmd(): impl{new_unique<Impl>()} {}
Cmd::~Cmd() {}
Str Cmd::last_command() const { return impl->last_command(); }
void Cmd::exec(CN<Str> cmd_and_args) { impl->exec(cmd_and_args); }
Strs Cmd::command_matches(CN<Str> cmd_and_args) const { return impl->command_matches(cmd_and_args); }
CN<Commands> Cmd::commands() const { return impl->commands(); }
void Cmd::move(Unique<Command>&& command) { impl->move(std::move(command)); }
