#pragma once
#include "cmd.hpp"

// список уровней
class Cmd_levels final: public Cmd::Command {
  Cmd* m_master {};
public:
  inline Cmd_levels(Cmd* master): m_master{master} {}
  ~Cmd_levels() = default;
  inline Str name() const override { return "levels"; }
  inline Str description() const override { return "print list of all game-levels"; }
  void exec(cr<Strs> cmd_and_args) override;
};

// выбрать уровень
class Cmd_level final: public Cmd::Command {
  Cmd* m_master {};
public:
  inline Cmd_level(Cmd* master): m_master{master} {}
  ~Cmd_level() = default;
  Str name() const override { return "level"; }
  inline Str description() const override {
    return "select level by level name (seed levels command output). "
      "example: level tutorial";
  }
  void exec(cr<Strs> cmd_and_args) override;
  Strs command_matches(cr<Strs> cmd_and_args) override;
};

// перезапустить уровень
class Cmd_restart final: public Cmd::Command {
  Cmd* m_master {};
public:
  inline Cmd_restart(Cmd* master): m_master{master} {}
  ~Cmd_restart() = default;
  inline Str name() const override { return "restart"; }
  inline Str description() const override { return "restart current game-level & reload resources"; }
  void exec(cr<Strs> cmd_and_args) override;
};
