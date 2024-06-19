#include "util/macro.hpp"
#include "util/str.hpp"
#include "cmd.hpp"

// выполнить команды из текстового файла
void execute_script(CN<Str> fname);

// выполняет команды из файла
class Cmd_script final: public Cmd::Command {
public:
  ~Cmd_script() = default;
  inline Str name() const override { return "script"; }
  inline Str description() const override {
    return "load and execute commands from .txt file\n"
      "example: script build/debug.txt"; 
  }
  void exec(CN<Strs> cmd_and_args) override;
};
