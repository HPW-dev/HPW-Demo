#include <ranges>
#include <utility>
#include "cmd.hpp"
#include "cmd-common.hpp"
#include "cmd-util.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/math/timer.hpp"
#include "game/core/messages.hpp"
#include "game/core/tasks.hpp"
#include "game/util/version.hpp"
#include "host/command.hpp"

void Cmd_exit::exec(cr<Strs> cmd_and_args) {
  iferror(!hpw::soft_exit, "hpw::soft_exit не инициализирован");
  hpw::soft_exit();
}

void Cmd_error::exec(cr<Strs> cmd_and_args) {
  // пустая ошибка
  if (cmd_and_args.size() <= 1)
    throw hpw::Error{};
  Str concated;
  for (std::size_t i = 1; i < cmd_and_args.size(); ++i)
    concated += cmd_and_args[i] + ' ';
  throw hpw::Error(concated);
}

void Cmd_print::exec(cr<Strs> cmd_and_args) {
  iferror(cmd_and_args.size() < 2, "need more parameters for print command");
  Str concated;
  for (std::size_t i = 1; i < cmd_and_args.size(); ++i)
    concated += cmd_and_args[i] + ' ';
  m_master->print(concated);
}

// для назначения новой команды
class Alias_helper final: public Cmd::Command {
  Cmd* m_master {};
  Str m_name {};
  Str m_cmd_and_args {};
public:
  inline explicit Alias_helper(Cmd* master, cr<Str> name, cr<Str> cmd_and_args)
    : m_master{master}
    , m_name{name}
    , m_cmd_and_args{cmd_and_args}
    {}
  ~Alias_helper() = default;
  inline Str name() const override { return m_name; }
  inline Str description() const override { return "alias for \"" + m_cmd_and_args + '\"'; }
  inline void exec(cr<Strs> cmd_and_args) override { m_master->exec(m_cmd_and_args); }
}; // Alias_helper

void Cmd_alias::exec(cr<Strs> cmd_and_args) {
  iferror(cmd_and_args.size() < 3, "need more parameters for alias command");
  crauto cmd_name = cmd_and_args.at(1);
  Str cmd_args;
  for (std::size_t i = 2; i < cmd_and_args.size(); ++i)
    cmd_args += cmd_and_args[i] + ' ';
  auto cmd = new_unique<Alias_helper>(m_master, cmd_name, cmd_args);
  m_master->move(std::move(cmd));
}

Strs Cmd_alias::command_matches(cr<Strs> cmd_and_args) {
  return_if (cmd_and_args.size() < 2, Strs{});

  cauto cmd = cmd_and_args.at(0);
  cauto alias_name = cmd_and_args.at(1);

  cauto matches = m_master->command_matches({});
  Strs ret;
  // без своего варианта команд предлагать любые команды
  if (cmd_and_args.size() == 2) {
    for (crauto it: matches)
      ret.push_back(cmd + ' ' + alias_name + ' ' + it);
    return ret;
  }

  // фильтровать имена команд по вводимому имени
  cauto second_cmd = cmd_and_args.at(2);
  cauto name_filter = [&](cr<Str> it)
    { return it.find(second_cmd) == 0; };
  for (crauto it: matches | std::views::filter(name_filter))
    ret.push_back(cmd + ' ' + alias_name + ' ' + it);
  return ret;
}

void Cmd_log_cnosole::exec(cr<Strs> cmd_and_args) {
  iferror(cmd_and_args.size() < 2, "need more parameters for log_console command");
  const bool yesno = s2n<int>(cmd_and_args.at(1)) == 0 ? false : true;
  m_master->enable_log_console(yesno);
}

void Cmd_log_screen::exec(cr<Strs> cmd_and_args) {
  iferror(cmd_and_args.size() < 2, "need more parameters for log_screen command");
  const bool yesno = s2n<int>(cmd_and_args.at(1)) == 0 ? false : true;
  m_master->enable_log_screen(yesno);
}

void Cmd_help::exec(cr<Strs> cmd_and_args) {
  Str text = "Commands info:\n";
  for (crauto command: m_master->commands())
    text += "* " + command->name() + " - " +
      command->description() + '\n';
  m_master->print(text);
}

void Cmd_cls::exec(cr<Strs> cmd_and_args) {
  iferror(!hpw::message_mgr, "hpw::message_mgr не инициализирован");
  hpw::message_mgr->clear();
}

void print_ver(Cmd_maker& command, Cmd& console, cr<Strs> args) {
  console.print(Str("game version: ") + get_game_version() + " " +
    + get_game_creation_date() + " " + get_game_creation_time());
}

class Task_drep final: public Task {
  Cmd& _console;
  int _times {};
  Timer _delay {};
  Str _cmd_with_args {};

public:
  inline explicit Task_drep(Cmd& console, int times, Delta_time delay, cr<Str> cmd_with_args)
  : _console {console}
  , _times {times}
  , _delay (delay)
  , _cmd_with_args {cmd_with_args}
  {
    // все проверки уже сделаны в drep
  }

  inline void update(const Delta_time dt) override {
    cfor (_, _delay.update(dt)) {
      _console.exec(_cmd_with_args);
      --_times;
    }

    if (_times <= 0)
      this->kill();
  }  
}; // Task_drep

void drep(Cmd_maker& command, Cmd& console, cr<Strs> args) {
  iferror(args.size() == 1, "не задано число повторений");
  iferror(args.size() == 2, "не задана задержка");
  iferror(args.size() < 4, "не задана команда для повторения");
  cauto times = s2n<int>(args[1]);
  iferror(times < 1, "число повторений не должно быть меньше 1");
  iferror(times > 4'000'000, "число повторений не должно быть больше 4M");
  cauto delay = s2n<Delta_time>(args[2]);
  iferror(delay < 0, "задержка должна быть положительной");

  // соединить оставшиеся аргументы в команду чтобы запустить их
  Str cmd_with_args;
  for (std::size_t i = 3; i < args.size(); ++i)
    cmd_with_args += args[i] + ' ';

  hpw::task_mgr.add(new_shared<Task_drep>(console, times, delay, cmd_with_args));
  console.print("команда \"" + cmd_with_args + "\" поставлена на повторение " +
    n2s(times) + " раз каждые " + n2s(delay, 3) + " секунд");
}

void cmd_common_init(Cmd& cmd) {
  #define MAKE_CMD(NAME, DESC, EXEC_F, MATCH_F) \
    cmd.move( new_unique<Cmd_maker>(cmd, NAME, DESC, EXEC_F, \
      Cmd_maker::Func_command_matches{MATCH_F}) );

  MAKE_CMD("version", "print game version", &print_ver, {})
  MAKE_CMD("drep", " drep <n> <delay_sec> <cmd> <args...> - delayed repeating of command", &drep, {})

  #undef MAKE_CMD
}
