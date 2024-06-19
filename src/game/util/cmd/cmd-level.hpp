#pragma once
#include "cmd.hpp"

// список уровней
class Cmd_levels final: public Cmd::Command {
  Cmd* m_master {};
public:
  inline Cmd_levels(Cmd* master): m_master{master} {}
  ~Cmd_levels() = default;
  inline Str name() const override { return "levels"; }
  inline utf32 description() const override { return U"print list of all game-levels"; }
  void exec(CN<Strs> cmd_and_args) override;
};

// выбрать уровень
class Cmd_set_level final: public Cmd::Command {
  Cmd* m_master {};
public:
  inline Cmd_set_level(Cmd* master): m_master{master} {}
  ~Cmd_set_level() = default;
  Str name() const override { return "set_level"; }
  inline utf32 description() const override {
    return U"select level by level name (seed levels command output)\n"
      "  example: set_level tutorial";
  }
  void exec(CN<Strs> cmd_and_args) override;
  Strs command_matches(CN<Strs> cmd_and_args) override;
};

// перезапустить уровень
class Cmd_restart final: public Cmd::Command {
  Cmd* m_master {};
public:
  inline Cmd_restart(Cmd* master): m_master{master} {}
  ~Cmd_restart() = default;
  inline Str name() const override { return "restart"; }
  inline utf32 description() const override { return U"restart current game-level"; }
  void exec(CN<Strs> cmd_and_args) override;
};
