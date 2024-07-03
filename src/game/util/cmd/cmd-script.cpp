#include <fstream>
#include "cmd-script.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "game/core/common.hpp"
#include "game/core/tasks.hpp"

// пошагово выполняет команды
class Exec_script final: public Task {
  std::ifstream m_file {};
  Str m_fname {};

public:
  inline Exec_script(CN<Str> _fname) {
    m_fname = _fname;
    conv_sep(m_fname);
  }

  inline void on_start() {
    m_file = std::ifstream(m_fname);

    if (!m_file.is_open()) {
      m_fname = hpw::cur_dir + m_fname;
      m_file = std::ifstream(m_fname);
      if (!m_file.is_open()) {
        hpw::cmd.print("файл скрипта \"" + m_fname
          + "\" не найден.\n" + "Игнор исполнения скрипта");
        return;
      }
    }
    hpw::cmd.print("запуск скрипта \"" + m_fname + '\"');

    Str line;
    while (std::getline(m_file, line))
      hpw::cmd.exec(line);
  }

  inline void on_end() {

  }

  inline void update(const Delta_time dt) {

  }
}; // Exec_script

void Cmd_script::exec(CN<Strs> cmd_and_args) {
  iferror(cmd_and_args.size() < 2, "need more parameters in script command");
  cauto fname = cmd_and_args.at(1);
  hpw::task_mgr.move( new_shared<Exec_script>(fname) );
}
