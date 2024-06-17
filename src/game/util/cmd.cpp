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
#include "util/math/random.hpp"
#include "host/command.hpp"
#include "game-archive.hpp"
#include "game/entity/ability/home.hpp"
#include "game/entity/ability/fullscreen-shoot.hpp"
#include "game/entity/ability/invise.hpp"
#include "game/entity/ability/power-shoot.hpp"
#include "game/entity/ability/speedup.hpp"
#include "game/core/messages.hpp"
#include "game/core/entities.hpp"
#include "game/core/canvas.hpp"
#include "game/entity/player.hpp"

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
      cauto pos_x_str = str_tolower(args.at(2));
      cauto pos_y_str = str_tolower(args.at(3));
      // если R за место кордов, то взять рандомную позицию с экрана
      const real pos_x = pos_x_str == "r"
        ? rndr(0, graphic::width)
        : s2n<real>(pos_x_str);
      const real pos_y = pos_y_str == "r"
        ? rndr(0, graphic::height)
        : s2n<real>(pos_y_str);
      pos = {pos_x, pos_y};
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
  
  struct Ability_info {
    Str name {};
    using Maker = std::function< Shared<Ability> (CN<Player> player) >;
    Maker maker {};
  };
  using Ability_infos = Vector<Ability_info>;

  inline static CN<Ability_infos> get_abilities() {
    sconst Ability_infos ret {
      {.name="home", .maker=[](CN<Player> player){ return new_shared<Ability_home>(player); }},
      {.name="fullscreen_shoot", .maker=[](CN<Player> player){ return new_shared<Ability_fullscreen_shoot>(player); }},
      {.name="invise", .maker=[](CN<Player> player){ return new_shared<Ability_invise>(player); }},
      {.name="power_shoot", .maker=[](CN<Player> player){ return new_shared<Ability_power_shoot>(player); }},
      {.name="speedup", .maker=[](CN<Player> player){ return new_shared<Ability_speedup>(player); }},
    };
    return ret;
  }

  inline static void print_entities_list(CN<Strs> args) {
    cauto list = get_entities_list();
    utf32 text = U"Avaliable entities:\n";
    for (cnauto name: list)
      text +=  U"- " + sconv<utf32>(name) + U'\n';
    print(text);
  }
  
  inline static void print_abilities(CN<Strs> args) {
    cauto list = get_abilities();
    utf32 text = U"Avaliable abilities:\n";
    for (cnauto it: list)
      text += U"- " + sconv<utf32>(it.name) + U'\n';
    print(text);
  }

  // назначает способность игроку
  inline static void add_ability(CN<Strs> args) {
    iferror(args.size() < 2, "need more params in add_ability command");
    auto player = hpw::entity_mgr->get_player();
    return_if(!player);
    
    // найти абилку из списка
    cnauto ability_name = args.at(1);
    cnauto abilities = get_abilities();
    auto it = std::find_if( abilities.begin(), abilities.end(),
      [&](CN<Ability_info> info) { return info.name == ability_name; } );
    iferror(it == abilities.end(), "not finded ability \"" << ability_name << "\"");

    // дать абилку игроку
    player->move_ability(std::move( it->maker(*player) ));
    print(U"ability \"" + sconv<utf32>(ability_name) + U"\" added to player");
  } // add_ability

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
          U"example: spawn enemy.snake.head 256 10"
          U"example random pos: spawn enemy.snake.head R R",
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
      Command {
        .name = "abilities",
        .description = U"print list of all avaliable player abilities",
        .action = &print_abilities
      },
      Command {
        .name = "add_ability",
        .description = U"add ability to player (powerup ability if repeated)",
        .action = &add_ability
      },
      Command {
        .name = "remove_abilities",
        .description = U"remove all player abilities",
        .action = [](CN<Strs> args) {
          auto player = hpw::entity_mgr->get_player();
          return_if(!player);
          player->remove_abilities();
          print(U"all player abilities removed");
        }
      },
      Command {
        .name = "cls",
        .description = U"clear all messages in game screen",
        .action = [](CN<Strs> args) { hpw::message_mgr->clear(); }
      },
      Command {
        .name = "exit",
        .description = U"shutdown game",
        .action = [](CN<Strs> args) { hpw::soft_exit(); }
      },
    }; // init m_commands
    // отсортировать команды по именам
    cauto name_sorter = [](CN<Command> a, CN<Command> b)->bool
      { return a.name < b.name; };
    std::sort(m_commands.begin(), m_commands.end(), name_sorter);
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
    } // cmd_name == spawn

    // дополнить команду add_ability вариантами абилок
    if (cmd_name == "add_ability") {
      cauto abilities = get_abilities();

      // если в слове есть часть введёной строки
      cauto ability_name_filter = [&](CN<Ability_info> it)->bool {
        if (args.size() > 1) {
          cauto ability_name = args.at(1);
          return it.name.find(ability_name) != Str::npos;
        }
        // если имени для спавна ещё нет, пропускать любое имя из списка
        return true;
      }; // ability_name_filter

      ret.clear();
      for (cnauto it: abilities | std::views::filter(ability_name_filter))
        ret.push_back(cmd_name + " " + it.name);
    } // cmd_name == add_ability

    return ret;
  } // command_matches

  inline Str last_command() const { return m_last_cmd; }
}; // Impl

Cmd::Cmd(): impl{new_unique<Impl>()} {}
Cmd::~Cmd() {}
void Cmd::exec(CN<Str> command) { impl->exec(command); }
Str Cmd::last_command() const { return impl->last_command(); }
Strs Cmd::command_matches(CN<Str> command) const { return impl->command_matches(command); }
