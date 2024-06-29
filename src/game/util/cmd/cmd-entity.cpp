#include <cassert>
#include <ranges>
#include "cmd-entity.hpp"
#include "cmd-util.hpp"
#include "game/core/entities.hpp"
#include "game/util/game-archive.hpp"
#include "game/util/game-util.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/file/yaml.hpp"

void spawn(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "не задано имя объекта в команде spawn");
  cnauto entity_name = args[1];

  Vec pos;
  if (args.size() >= 4) {
    pos.x = s2n<real>(args[2]);
    pos.y = s2n<real>(args[3]);
  } else {
    pos = rnd_screen_pos_safe();
  }

  hpw::entity_mgr->make({}, entity_name, pos);
  console.print("entity \"" + entity_name +
    "\" spawned at {" + n2s(pos.x, 2) + ", " +
    n2s(pos.y, 2) + '}');
}

Strs get_entity_names() {
  const Yaml config(hpw::archive->get_file("config/entities.yml"));
  Strs ret;
  for (cnauto tag: config.root_tags())
    ret.push_back(tag);
  return ret;
}

// предложить имена всего что можно соспавнить
Strs spawn_matches(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  return_if(args.size() > 2, Strs{});
  Strs ret;
  cauto cmd_name = ctx.name();
  cauto entity_names = get_entity_names();

  // отфильтровать пользоватеьский ввод
  if (args.size() == 2) {
    cnauto entity_name = args.at(1);
    cauto name_filter = [&](CN<Str> it)
      { return it.find(entity_name) == 0; };
    for (cnauto name: entity_names | std::views::filter(name_filter))
      ret.push_back(cmd_name + ' ' + name);
    return ret;
  }

  // предложить из списка
  for (cnauto name: entity_names)
    ret.push_back(cmd_name + ' ' + name);
  return ret;
}

void cmd_entity_init(Cmd& cmd) {
  #define MAKE_CMD(NAME, DESC, EXEC_F, MATCH_F) \
    cmd.move( new_unique<Cmd_maker>(cmd, NAME, DESC, EXEC_F, \
      Cmd_maker::Func_command_matches{MATCH_F}) );

  MAKE_CMD (
    "spawn",
    "spawn <entity> <x> <y> - make entity on x/y pos."
    "If x/y not defined, use random on-screen pos",
    &spawn, &spawn_matches )
    
  #undef MAKE_CMD
} // cmd_entity_init
