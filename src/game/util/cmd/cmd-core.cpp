#include <cassert>
#include <chrono>
#include <fstream>
#include "cmd-util.hpp"
#include "game/core/graphic.hpp"
#include "game/core/tasks.hpp"
#include "game/core/entities.hpp"
#include "game/core/core.hpp"
#include "game/util/sync.hpp"
#include "game/util/config.hpp"
#include "host/host-util.hpp"
#include "util/math/timer.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"

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

using namespace std::chrono_literals;

class Task_state_saver final: public Task {
  Str m_fname {};
  std::ofstream m_file {};
  Cmd& m_console;
  struct Stat {
    Str title {};
    using Getter = std::function<Str ()>;
    Getter getter {};
  };
  Vector<Stat> m_stats {};
  cautox separator = '|';
  Delta_time m_timeout {};
  std::chrono::steady_clock::time_point m_time_start {};
  std::chrono::steady_clock::time_point m_sample_delay_start {};
  cautox sample_delay {1s};

public:
  inline explicit Task_state_saver(CN<Str> fname, const Delta_time timeout,
  Cmd& console)
  : m_fname {fname}
  , m_console {console}
  , m_timeout {timeout}
  {
    iferror(fname.empty(), "пустое имя файла");
    iferror(timeout <= 0, "неправильный параметр таймаута");
    m_stats = {
      Stat {.title="'Software draw time Ms.'", .getter=[]{
        return n2s(graphic::soft_draw_time * 1'000.f, 5); }},
      Stat {.title="'Hardware draw time Ms.'", .getter=[]{
        return n2s(graphic::hard_draw_time * 1'000.f, 5); }},
      Stat {.title="'tick time Ms.'", .getter=[]{
        return n2s(hpw::update_time_unsafe * 1'000.f, 5); }},
      Stat {.title="'Real Dt. Ms.'", .getter=[]{
        return n2s(hpw::real_dt * 1'000.f, 5); }},
      Stat {.title="'FPS'", .getter=[]{
        return n2s(graphic::cur_fps, 2); }},
      Stat {.title="'tickrate'", .getter=[]{
        return n2s(hpw::cur_ups, 2); }},
      Stat {.title="'Lives'", .getter=[]{
        cnauto entities = hpw::entity_mgr->get_entities();
        uint lives {};
        for (cnauto entity: entities)
          lives += entity->status.live;
        return n2s(lives);
      }},
      Stat {.title="'Entities capacity'", .getter=[]{
        return n2s(hpw::entity_mgr->get_entities().capacity()); }},
    };
  } // c-tor

  inline void on_start() override {
    m_file.open(m_fname);
    iferror(!m_file.is_open(), "не удалось открыть файл \"" << m_fname << "\"");
    Str grid_title;
    for (cnauto stat: m_stats)
      grid_title += stat.title + separator;
    m_file << grid_title << std::endl;
    m_sample_delay_start = m_time_start = std::chrono::steady_clock::now();
  }

  inline void update(const Delta_time dt) override {
    using Seconds = std::chrono::duration<Delta_time, std::ratio<1, 1>>;
    // проверить таймер выхода
    cauto timeout_timediff = std::chrono::steady_clock::now() - m_time_start;
    if (std::chrono::duration_cast<Seconds>(timeout_timediff).count()
    >= m_timeout) {
      kill();
      return;
    }
    
    // каждую секунду сейвить инфу о игре
    cauto sample_delay_timediff = std::chrono::steady_clock::now()
      - m_sample_delay_start;
    if (std::chrono::duration_cast<Seconds>(sample_delay_timediff)
    >= sample_delay) {
      for (cnauto stat: m_stats)
        m_file << stat.getter() + separator;
      m_file << std::endl;
      m_sample_delay_start = std::chrono::steady_clock::now();
    }
  }

  inline void on_end() override {
    m_file.close();
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
