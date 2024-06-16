#include <fstream>
#include "file-cmd.hpp"
#include "util/log.hpp"
#include "game/util/cmd.hpp"

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
