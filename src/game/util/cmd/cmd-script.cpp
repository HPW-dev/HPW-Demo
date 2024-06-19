#include <fstream>
#include "cmd-script.hpp"
#include "cmd.hpp"
#include "util/log.hpp"

void execute_script(CN<Str> fname) {
  std::ifstream file(fname);
  if (!file || !file.good()) {
    hpw_log (
      "файл скрипта \"" << fname << "\" не найден.\n" <<
      "Игнор исполнения скрипта\n" );
    return;
  }

  Str line;
  while (std::getline(file, line))
    hpw::cmd->exec(line);
}
