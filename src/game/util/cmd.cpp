#include <filesystem>
#include <utility>
#include <ranges>
#include <algorithm>
#include <functional>
#include "cmd.hpp"
#include "util/vector-types.hpp"
#include "util/str-util.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/file/yaml.hpp"
#include "game-archive.hpp"
#include "game/core/messages.hpp"
#include "game/core/entities.hpp"

struct Cmd::Impl {
  struct Command {
    using Action = std::function<void (CN<Strs> args)>;
    Str name {};
    utf32 description {};
    Action action {};
  };

  Vector<Command> m_commands {};
  Str m_last_cmd {}; // предыдущая команда

  inline Impl() {
    init_commands();
  } // c-tor

  inline void exec(CN<Str> command_str) {
    m_last_cmd = command_str;
    try {
      cauto splited = split_str(command_str, ' ');
      cnauto command = find_command(splited.at(0));
      command.action(splited);
    } catch (CN<hpw::Error> err) {
      print(U"ошибка при выполнении команды \"" +
        sconv<utf32>(command_str) + U"\":\n" + sconv<utf32>(err.what()));
    } catch (...) {
      print(U"неизвестная ошибка при выполнении команды \"" +
        sconv<utf32>(command_str) + U"\"");
    }
  }

  inline CN<Command> find_command(CN<Str> name) const {
    cauto lower_name = str_tolower(name);
    cnauto finded_cmd = std::find_if(m_commands.begin(), m_commands.end(),
      [&](CN<Command> cmd) { return cmd.name == lower_name; });
    if (finded_cmd != m_commands.end())
      return *finded_cmd;
    error("not finded command \"" << name << "\"");
    return *(Command*){}; // заглушка для анализатора
  }

  // справка о всех командах
  inline void help(CN<Strs> args) {
    utf32 text = U"List commands:\n";
    for (cnauto command: m_commands) {
      text += U"* " + sconv<utf32>(command.name) + U" - ";
      text += command.description;
      text += U";\n";
    }
    print(text);
  }

  // спавнит сущность
  inline static void spawn(CN<Strs> args) {
    iferror(args.size() < 2, "need more params in spawn command");
    cauto entity_name = args.at(1);

    Vec pos;
    if (args.size() >= 4) {
      pos = {
        s2n<real>(args.at(2)),
        s2n<real>(args.at(3))
      };
    } else { // если корды не заданы, создать объект сверху посередине
      pos = {256, 50};
    }

    hpw::entity_mgr->make({}, entity_name, pos);
    print(U"spawned \"" + sconv<utf32>(entity_name) +
      U"\" at {" + n2s<utf32>(pos.x, 2) + U", " + n2s<utf32>(pos.y, 2) + U"}");
  }

  // пишет текст в консоль и на экран
  inline static void print(CN<utf32> text) {
    // вывести в консоль
    hpw_log(sconv<Str>(text) << '\n');
    // вывести на экран игры
    Message msg;
    msg.text = text;
    msg.lifetime = 3.5;
    hpw::message_mgr->move(std::move(msg));
  }

  // пишет текст в консоль и на экран
  inline static void echo(CN<Strs> args) {
    iferror(args.size() < 2, "need more params in print (echo)");
    utf32 text = U"сообщение: ";
    for (uint i = 1; i < args.size(); ++i)
      text += sconv<utf32>(args.at(i)) + U' ';
    print(text);
  }

  inline static Strs get_entities_list() {
    const Yaml config(hpw::archive->get_file("config/entities.yml"));
    Strs ret;
    for (cnauto tag: config.root_tags())
      ret.push_back(tag);
    return ret;
  }

  inline static void print_entities_list(CN<Strs> args) {
    cauto list = get_entities_list();
    utf32 text = U"Avaliable entities:\n";
    for (cnauto name: list)
      text += sconv<utf32>(name) + U'\n';
    print(text);
  }

  inline void init_commands() {
    m_commands = Vector<Command> {
      Command {
        .name = "help",
        .description = U"print this help",
        .action = [this](CN<Strs> args) { help(args); }
      },
      Command {
        .name = "spawn",
        .description =
          U"spawn <entity name> <pos x> <pos y>\n"
          U"example: spawn enemy.snake.head 256 10",
        .action = &spawn
      },
      Command {
        .name = "print",
        .description = U"print <text>",
        .action = &echo
      },
      Command {
        .name = "entities",
        .description = U"print list of all avaliable entities for spawn",
        .action = &print_entities_list
      },
    };
  } // init_commands

  inline Strs command_matches(CN<Str> command) const {
    Strs ret;

    // если нет параметров, показать все команды и выйти
    if (command.empty()) {
      for (cnauto command: m_commands)
        ret.push_back(command.name);
      return ret;
    }

    cauto args = split(command, ' ');
    cauto cmd_name = str_tolower( args.at(0) );
    // учитывать совпадения в начале слова
    auto command_match = [&](CN<Command> arg)->bool
      { return arg.name.find(cmd_name) == 0; };
    // найти совпадающие команды по их названию
    for (cnauto founded: m_commands | std::views::filter(command_match))
      ret.push_back(founded.name);
    
    // дополнить команду spawn вариантами entity
    if (cmd_name == "spawn") {
      cauto entities = get_entities_list();

      // если в слове есть часть введёной строки
      cauto entity_name_filter = [&](CN<Str> name)->bool {
        if (args.size() > 1) {
          cauto entity_name = args.at(1);
          return name.find(entity_name) != Str::npos;
        }
        // если имени для спавна ещё нет, пропускать любое имя из списка
        return true;
      }; // entity_name_filter

      ret.clear();
      for (cnauto name: entities | std::views::filter(entity_name_filter))
        ret.push_back(cmd_name + " " + name);
    }

    return ret;
  } // command_matches

  inline Str last_command() const { return m_last_cmd; }
}; // Impl

Cmd::Cmd(): impl{new_unique<Impl>()} {}
Cmd::~Cmd() {}
void Cmd::exec(CN<Str> command) { impl->exec(command); }
Str Cmd::last_command() const { return impl->last_command(); }
Strs Cmd::command_matches(CN<Str> command) const { return impl->command_matches(command); }
