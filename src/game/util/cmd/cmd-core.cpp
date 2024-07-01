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
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/math/random.hpp"
#include "util/math/timer.hpp"

//#include "util/log.hpp"

// выполняет команду после задержки
class Timed_cmd final: public Task {
  Cmd& m_console;
  Str m_cmd_with_args {};
  Delta_time m_delay {};
  Delta_time m_delay_start {};

public:
  inline explicit Timed_cmd(Cmd& console, Delta_time delay,
  CN<Str> cmd_with_args)
  : m_console {console}
  , m_cmd_with_args {cmd_with_args}
  , m_delay {delay}
  , m_delay_start {delay}
  {
    iferror(delay < 0, "задержка < 0");
    iferror(cmd_with_args.empty(), "команда пуста");
  }

  inline void update(const Delta_time dt) override {
    //hpw_log("delay: " << m_delay << '/' << m_delay_start << '\n');
    if (m_delay <= 0)
      kill(); // в on_end вызовется команда
    m_delay -= dt;
  }

  inline void on_end() override { m_console.exec(m_cmd_with_args); }
}; // Timed_cmd

void timed(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 3, "в команде timed недостаточно аргументов");
  cauto delay = s2n<real>(args[1]);
  // объединить строки в команду для отложенного запуска
  Str cmd_str;
  for (std::size_t i = 2; i < args.size(); ++i)
    cmd_str += '\"' + args.at(i) + "\" ";
  hpw::task_mgr.move( new_shared<Timed_cmd>(console, delay, cmd_str) );
  console.print(Str("команда \"") + cmd_str + "\" поставлена на таймер");
}

void set_seed(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "в команде seed не задан параметр");
  cnauto seed = s2n<std::uint32_t>(args[1]);
  set_rnd_seed(seed);
  console.print("новый сид рандома = " + n2s(seed));
}

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
  Delta_time m_sample_delay {};
  std::chrono::steady_clock::time_point m_time_start {};
  std::chrono::steady_clock::time_point m_sample_delay_start {};

public:
  inline explicit Task_state_saver(CN<Str> fname, const Delta_time timeout,
  const Delta_time sample_delay, Cmd& console)
  : m_fname {fname}
  , m_console {console}
  , m_timeout {timeout}
  , m_sample_delay {sample_delay}
  {
    iferror(fname.empty(), "пустое имя файла");
    iferror(m_timeout <= 0, "неправильный параметр таймаута");
    iferror(m_sample_delay <= 0,
      "неправильный параметр задержки между сохранениями");
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
    if (std::chrono::duration_cast<Seconds>(sample_delay_timediff).count()
    >= m_sample_delay) {
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
  iferror(args.size() < 4, "в команде stat задано мало параметров");
  cnauto filename = args[1];
  cnauto seconds = s2n<Delta_time>(args[2]);
  cnauto sample_delay = s2n<Delta_time>(args[3]);
  if (::g_state_saver_task)
    end_stat_record(command, console, args);
  ::g_state_saver_task = hpw::task_mgr.move (
    new_shared<Task_state_saver>(filename, seconds, sample_delay, console) );
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
    "tickrate <limit> - sets count of updates per sec.",
    &set_tickrate, {} )
  MAKE_CMD (
    "stats",
    "stats <filename> <seconds> <sample save delay>"
    " - save statistics to file. ",
    &start_stat_record, {} )
  MAKE_CMD (
    "stats_end",
    "stats_end - end of saving statistics to file",
    &end_stat_record, {} )
  MAKE_CMD (
    "config_reload",
    "config_reload - reload config.yml",
    &config_reload, {} )
  MAKE_CMD (
    "seed",
    "seed <num> - set random seed",
    &set_seed, {} )
  MAKE_CMD (
    "timed",
    "timed <seconds> <command args...> - execute command after <seconds>",
    &timed, {} )
    
  #undef MAKE_CMD
} // cmd_core_init
