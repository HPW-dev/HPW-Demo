#include <utility>
#include <algorithm>
#include <functional>
#include "cmd.hpp"
#include "util/vector-types.hpp"
#include "util/str-util.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
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

  inline Impl() {
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
    }; // init commands
  } // c-tor

  inline void exec(CN<Str> command_str) {
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
    iferror(args.size() < 4, "мало параметров в функции spawn");
    cauto entity_name = args.at(1);
    const Vec pos {
      s2n<real>(args.at(2)),
      s2n<real>(args.at(3))
    };
    cauto ret = hpw::entity_mgr->make({}, entity_name, pos);
    print(U"spawned \"" + sconv<utf32>(ret->name()) +
      U"\" at {" + n2s<utf32>(pos.x, 2) + U", " + n2s<utf32>(pos.y, 2) + U"}");
  }

  // пишет текст в консоль и на экран
  inline static void print(CN<utf32> text) {
    // вывести в консоль
    hpw_log(sconv<Str>(text) << '\n');
    // вывести на экран игры
    Message msg;
    msg.text = text;
    msg.lifetime = 4;
    hpw::message_mgr->move(std::move(msg));
  }
}; // Impl

Cmd::Cmd(): impl{new_unique<Impl>()} {}
Cmd::~Cmd() {}
void Cmd::exec(CN<Str> command) { impl->exec(command); }
