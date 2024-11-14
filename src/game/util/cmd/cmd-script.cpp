#include <fstream>
#include "cmd-script.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "game/core/common.hpp"
#include "game/core/tasks.hpp"
#include "game/core/entities.hpp"
#include "game/core/core.hpp"

// пошагово выполняет команды
class Exec_script final: public Task {
  std::ifstream m_file {};
  Str m_fname {};

public:
  inline Exec_script(cr<Str> _fname): m_fname {_fname}
    { conv_sep(m_fname); }

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
  }

  inline void on_end() {
    hpw::cmd.print("завершение скрипта \"" + m_fname + '\"');
    m_fname.clear();
    m_file.close();
  }

  inline void update(const Delta_time dt) {
    /* читать и выполнять строки из файла 
    по порядку, в конце завершить процесс */
    Str line;
    crauto status = std::getline(m_file, line);
    if (status)
      hpw::cmd.exec(line);
    else
      kill();
  }
}; // Exec_script

// мгновенно выполняет все команды из файла
class Exec_script_instant final: public Task {
  Str m_fname {};

public:
  inline Exec_script_instant(cr<Str> fname): m_fname {fname}
    { conv_sep(m_fname); }

  inline void on_end() { m_fname.clear(); }

  inline void update(const Delta_time dt) {
    std::ifstream m_file = std::ifstream(m_fname);

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

    /* читать и выполнять строки из файла 
    по порядку, в конце завершить процесс */
    Str line;
    while (std::getline(m_file, line)) {
      hpw::cmd.exec(line);
      hpw::entity_mgr->update(hpw::target_update_time);
    }
      
    kill();
    m_file.close();
    hpw::cmd.print("завершение скрипта \"" + m_fname + '\"');
  }
}; // Exec_script_instant

void Cmd_script::exec(cr<Strs> cmd_and_args) {
  iferror(cmd_and_args.size() < 2, "need more parameters in script command");
  cauto fname = cmd_and_args.at(1);
  hpw::task_mgr.add( new_shared<Exec_script>(fname) );
}

void Cmd_script_instant::exec(cr<Strs> cmd_and_args) {
  iferror(cmd_and_args.size() < 2, "need more parameters in script_instant command");
  cauto fname = cmd_and_args.at(1);
  hpw::task_mgr.add( new_shared<Exec_script_instant>(fname) );
}
