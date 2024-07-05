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
