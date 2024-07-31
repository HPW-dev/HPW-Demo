#include <cassert>
#include <chrono>
#include <ranges>
#include <fstream>
#include "cmd-util.hpp"
#include "game/core/graphic.hpp"
#include "game/core/tasks.hpp"
#include "game/core/entities.hpp"
#include "game/core/core.hpp"
#include "game/core/debug.hpp"
#include "game/core/canvas.hpp"
#include "game/util/sync.hpp"
#include "game/util/config.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/entity/collider/collider-empty.hpp"
#include "game/entity/collider/collider-simple.hpp"
#include "game/entity/collider/collider-qtree.hpp"
#include "game/entity/collider/collider-grid.hpp"
#include "game/entity/collider/collider-2d-tree.hpp"
#include "game/entity/collider/collider-experimental.hpp"
#include "game/entity/collider/collider-experimental-2.hpp"
#include "host/host-util.hpp"
#include "util/hpw-util.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/math/random.hpp"
#include "util/math/timer.hpp"

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

  #ifdef DETAILED_LOG
    console.print(Str("команда \"") + cmd_str + "\" поставлена на таймер");
  #endif
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
        return n2s(hpw::tick_time * 1'000.f, 5); }},
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

// повторяет команды
void repeat(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() == 1, "в команде repeat не задано число повторений");
  iferror(args.size() < 3, "в команде repeat не задана команда для повторения");
  cnauto times = s2n<int>(args[1]);
  iferror(times < 1, "число повторений не должно быть меньше 1");
  iferror(times > 4'000'000, "число повторений не должно быть больше 4M");

  // соединить оставшиеся аргументы в команду чтобы запустить их
  Str cmd_with_args;
  for (std::size_t i = 2; i < args.size(); ++i)
    cmd_with_args += args[i] + ' ';

  cfor (i, times)
    console.exec(cmd_with_args);
}

void clear_tasks(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  hpw::task_mgr.clear();
  console.print("все задачи удалены");
}

void set_direction(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "в команде direct мало аргументов");
  cauto uid_a = s2n<Uid>(args[1]);
  cauto uid_b = s2n<Uid>(args[2]);

  auto a = hpw::entity_mgr->find(uid_a);
  auto b = hpw::entity_mgr->find(uid_b);
  iferror(!a, "не удалось найти объект с UID = " << uid_a);
  iferror(!b, "не удалось найти объект с UID = " << uid_b);

  cauto deg = deg_to_target(*a, *b);
  a->phys.set_deg(deg);

  cauto name_a = a->name();
  cauto name_b = b->name();
  console.print("объект \"" + name_a
    + "\" направлен на объект \"" + name_b + "\"");
}

void set_speed(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "в команде speed мало аргументов");
  cauto uid = s2n<Uid>(args[1]);
  auto speed = s2n<real>(args[2]);
  speed = pps(speed);

  auto entity = hpw::entity_mgr->find(uid);
  iferror(!entity, "не удалось найти объект с UID = " << uid);

  entity->phys.set_speed(speed);
  console.print("объекту \"" + entity->name()
    + "\" назначена скорость " + n2s(speed, 2));
}

void draw_hitboxes(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "в команде мало аргументов");
  cauto yesno = (args.at(1) == "0" ? false : true);
  #ifdef DEBUG
  graphic::draw_hitboxes = yesno;
  //graphic::draw_full_hitboxes = yesno;
  graphic::draw_entities = !yesno;
  graphic::draw_level = !yesno;
  #endif
  console.print("режим показа хитбоксов " + Str(yesno ? "включён" : "выключен"));
}

void draw_grids(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "в команде мало аргументов");
  cauto yesno = (args.at(1) == "0" ? false : true);
  graphic::show_grids = yesno;
  console.print("режим показа сетки " + Str(yesno ? "включён" : "выключен"));
}

// системы коллизий и их имена
using Collider_maker = std::function< Shared<Collider> ()>;
static std::unordered_map<Str, Collider_maker> g_colliders {
  {"empty", []{ return new_shared<Collider_empty>(); }},
  {"simple", []{ return new_shared<Collider_simple>(); }},
  {"experimental", []{ return new_shared<Collider_experimental>(); }},
  {"experimental-2", []{ return new_shared<Collider_experimental_2>(); }},
  {"qtree", []{ return new_shared<Collider_qtree>(7, 1, graphic::width, graphic::height); }},
  {"grid", []{ return new_shared<Collider_grid>(); }},
  {"2d-tree", []{ return new_shared<Collider_2d_tree>(); }},
};

void set_collider(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "need more parameters for collider command");
  cnauto collider_name = args.at(1);
  cnauto maker = g_colliders.at(collider_name);
  assert(hpw::entity_mgr);
  hpw::entity_mgr->set_collider(maker());
  console.print("selected \"" + collider_name + "\" collision resolver");
}

Strs set_collider_matches(Cmd_maker& command, Cmd& console, CN<Strs> args) {
  Strs ret;
  cauto cmd_name = args.at(0);
  if (args.size() < 2) {
    // предложить системы из списка
    for (cnauto [collider_name, maker]: g_colliders)
      ret.push_back(cmd_name + ' ' + collider_name);
    return ret;
  }

  // по вводу определить что взять из списка автодополнения
  cauto arg_name = args.at(1); // имя колайдер ресолвера
  cauto name_filter = [&](CN<decltype(g_colliders)::value_type> it)
    { return it.first.find(arg_name) == 0; };
  for (cnauto [collider_name, maker]: g_colliders | std::views::filter(name_filter))
    ret.push_back(cmd_name + ' ' + collider_name);
  return ret;
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
  MAKE_CMD (
    "rep",
    "rep <count> <command args...> - repeats command <count> times",
    &repeat, {} )
  MAKE_CMD (
    "clear_tasks",
    "clear_tasks - убирает все задачи",
    &clear_tasks, {} )
  MAKE_CMD (
    "direct",
    "direct <uid> <uid> - поворачивает один объект в сторону другого",
    &set_direction, {} )
  MAKE_CMD (
    "speed",
    "speed <uid> <speed> - поворачивает меняет скорость объекту",
    &set_speed, {} )
  MAKE_CMD (
    "hitboxes",
    "hitboxes <1/0> - вкл/выкл режим отображения хитбоксов",
    &draw_hitboxes, {} )
  MAKE_CMD (
    "grids",
    "grids <1/0> - вкл/выкл режим отображения сеток",
    &draw_grids, {} )
  MAKE_CMD (
    "collider",
    "collider <name> set collision resolver",
    &set_collider, &set_collider_matches )
    
  #undef MAKE_CMD
} // cmd_core_init
