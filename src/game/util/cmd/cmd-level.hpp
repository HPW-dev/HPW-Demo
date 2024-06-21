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
  void exec(CN<Strs> cmd_and_args) override;
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
  inline Str description() const override { return "restart current game-level & reload resources"; }
  void exec(CN<Strs> cmd_and_args) override;
};

// выбор обработчика столкновений
class Cmd_collider final: public Cmd::Command {
  Cmd* m_master {};
public:
  inline Cmd_collider(Cmd* master): m_master{master} {}
  ~Cmd_collider() = default;
  inline Str name() const override { return "collider"; }
  inline Str description() const override { return "collider <name> set collision resolver"; }
  void exec(CN<Strs> cmd_and_args) override;
  Strs command_matches(CN<Strs> cmd_and_args) override;
};
