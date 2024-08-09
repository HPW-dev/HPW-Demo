#pragma once
#include "cmd.hpp"

// выполняет команды из файла
class Cmd_script final: public Cmd::Command {
public:
  ~Cmd_script() = default;
  inline Str name() const override { return "script"; }
  inline Str description() const override {
    return "load and execute commands from .txt file. "
      "example: script build/debug.txt"; 
  }
  void exec(CN<Strs> cmd_and_args) override;
};

// выполняет команды из файла за один апдейт
class Cmd_script_instant final: public Cmd::Command {
public:
  ~Cmd_script_instant() = default;
  inline Str name() const override { return "script_instant"; }
  inline Str description() const override {
    return "script_instant <file.txt> - мгновенно выполняет команды из файла"; 
  }
  void exec(CN<Strs> cmd_and_args) override;
};
