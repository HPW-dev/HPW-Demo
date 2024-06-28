#include <cassert>
#include <fstream>
#include "cmd-util.hpp"
#include "game/core/graphic.hpp"
#include "game/core/tasks.hpp"
#include "host/host-util.hpp"
#include "game/util/sync.hpp"
#include "game/util/config.hpp"
#include "util/math/timer.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/log.hpp"

void set_fps_limit(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "не указано количество FPS в команде");
  cauto new_fps = s2n<int>(args[1]);

  if (new_fps <= 0) {
    graphic::set_disable_frame_limit(true);
    console.print("лимит FPS выключен");
  } else {
    graphic::set_disable_frame_limit(false);
    graphic::set_target_fps(new_fps);
    console.print("лимит FPS = " + n2s(new_fps));
  }
}

void config_reload(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  load_config(); 
  console.print("конфиг перезагружен");
}

void enable_render(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "у команды "
    + command.name() + " не задан параметр");
  const bool yesno = s2n<int>(args[1]) == 0 ? false : true;
  graphic::enable_render = yesno;
  console.print(Str("рендер ") + (yesno ? "включён" : "выключен"));
}

void set_tickrate(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "не указано количество UPS в команде");
  cauto new_ups = s2n<int>(args[1]);
  set_target_ups(new_ups);
  console.print("тикрейт игры = " + n2s(new_ups, 2));
}

class Task_state_saver final: public Task {
  Timer m_sample_delay {1};
  Timer m_timeout {};
  std::ofstream m_file {};
  Cmd& m_console;

public:
  inline explicit Task_state_saver(CN<Str> fname, const Delta_time _timeout,
  Cmd& console)
  : m_timeout {_timeout}
  , m_console {console}
  {
    iferror(_timeout <= 0, "неправильный параметр таймаута");
    m_file.open(fname);
    iferror(!m_file.is_open(), "не удалось открыть файл \"" + fname + "\"");
  }

  inline void update(const Delta_time dt) override {
    if (m_timeout.update(dt)) {
      kill();
      return;
    }
    // каждую секунду сейвить инфу о игре
    cfor (_, m_sample_delay.update(dt))
      hpw_log("test\n");
  }

  inline void on_end() override {
    m_console.print("сбор статистики завершён");
  }
}; // Task_state_saver

namespace {
Shared<Task> g_state_saver_task {};
}

void end_stat_record(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  if (::g_state_saver_task) {
    ::g_state_saver_task->kill();
    ::g_state_saver_task = {};
    console.print("сбор статистики принудительно завершён");
    return;
  }
  console.print("не удалось завершить запись статистики, "
    "так как она и не начиналась");
}

void start_stat_record(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 3, "в команде stat_record задано мало параметров");
  cnauto filename = args[1];
  cnauto seconds = s2n<Delta_time>(args[2]);
  if (::g_state_saver_task)
    end_stat_record(command, console, args);
  ::g_state_saver_task = hpw::task_mgr.move (
    new_shared<Task_state_saver>(filename, seconds, console) );
  console.print("Начат сбор статистики...");
  console.print("Статистика сохранится в файл \"" + filename + "\"");
  console.print("Завершение через " + n2s(seconds) + " сек.");
}

void cmd_core_init(Cmd& cmd) {
  #define MAKE_CMD(NAME, DESC, EXEC_F, MATCH_F) \
    cmd.move( new_unique<Cmd_maker>(cmd, NAME, DESC, EXEC_F, \
      Cmd_maker::Func_command_matches{MATCH_F}) );

  MAKE_CMD (
    "fps",
    "fps <limit> - set fps limit. If limit = 0, disable limit",
    &set_fps_limit, {} )
  MAKE_CMD (
    "render",
    "render <1/0> - enable/disable rendering",
    &enable_render, {} )
  MAKE_CMD (
    "tickrate",
    "tickrate <limit> - sets count of updates per. sec.",
    &set_tickrate, {} )
  MAKE_CMD (
    "stats_start",
    "stats_start <filename> <seconds> - save statistics to file. "
    "If seconds = 0 - infinite",
    &start_stat_record, {} )
  MAKE_CMD (
    "stats_end",
    "stats_end - end of saving statistics to file",
    &end_stat_record, {} )
  MAKE_CMD (
    "config_reload",
    "config_reload - reload config.yml",
    &config_reload, {} )
    
  #undef MAKE_CMD
} // cmd_core_init
