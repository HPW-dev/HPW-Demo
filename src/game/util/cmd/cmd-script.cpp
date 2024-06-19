#include <fstream>
#include "cmd-script.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "game/core/common.hpp"

void execute_script(CN<Str> _fname) {
  auto fname = _fname;
  conv_sep(fname);

  std::ifstream file(fname);
  if (!file.is_open()) {
    fname = hpw::cur_dir + fname;
    file = std::ifstream(fname);
    if (!file.is_open()) {
      hpw::cmd.print("файл скрипта \"" + fname
        + "\" не найден.\n" + "Игнор исполнения скрипта");
      return;
    }
  }
  hpw::cmd.print("запуск скрипта \"" + fname + '\"');

  Str line;
  while (std::getline(file, line))
    hpw::cmd.exec(line);
}

void Cmd_script::exec(CN<Strs> cmd_and_args) {
  iferror(cmd_and_args.size() < 2, "need more parameters in script command");
  cauto fname = cmd_and_args.at(1);
  execute_script(fname);
}
