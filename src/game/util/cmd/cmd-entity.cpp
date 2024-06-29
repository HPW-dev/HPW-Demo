#include <cassert>
#include <ranges>
#include <algorithm>
#include "cmd-entity.hpp"
#include "cmd-util.hpp"
#include "game/core/entities.hpp"
#include "game/util/game-archive.hpp"
#include "game/util/game-util.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/file/yaml.hpp"

namespace {
Uid last_uid {}; // последний соспавненный объект
}

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

  cauto entity = hpw::entity_mgr->make({}, entity_name, pos);
  ::last_uid = entity->uid;
  console.print("entity \"" + entity_name
    + "\" spawned at {" + n2s(pos.x, 2) + ", "
    + n2s(pos.y, 2) + "} uid: " + n2s(::last_uid));
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

void kill(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  cnauto entity_uid = args.size() >= 2 ? args[1] : n2s(::last_uid);
  cnauto entities = hpw::entity_mgr->get_entities();
  cauto it = std::find_if(entities.begin(), entities.end(),
    [&](CN<Entitys::value_type> entity) {
      return n2s(entity->uid) == entity_uid;
    }
  );

  if (it != entities.end()) {
    cnauto entity = it->get();
    entity->kill();
    console.print("killed entity \"" + entity->name()
      + "\" uid: " + entity_uid);
  } else {
    error("не удалось убить объект с UID = " << entity_uid);
  }
} // kill

// получить uid'ы всех живых объекто в виде строк
Strs get_lived_str_uid() {
  Strs ret;
  cnauto entityes = hpw::entity_mgr->get_entities();
  for (cnauto entity: entityes)
    if (entity->status.live)
      ret.push_back( n2s(entity->uid) );
  return ret;
}

// предложить uid'ы всего того, что можно убить
Strs kill_matches(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  return_if(args.size() > 2, Strs{});
  Strs ret;
  cauto cmd_name = ctx.name();
  cauto str_uids = get_lived_str_uid();

  // отфильтровать пользоватеьский ввод
  if (args.size() == 2) {
    cnauto entity_name = args.at(1);
    cauto name_filter = [&](CN<Str> it)
      { return it.find(entity_name) == 0; };
    for (cnauto name: str_uids | std::views::filter(name_filter))
      ret.push_back(cmd_name + ' ' + name);
    return ret;
  }

  // предложить из списка
  for (cnauto name: str_uids)
    ret.push_back(cmd_name + ' ' + name);
  return ret;
}

// убирает всех противников
void clear_entities(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  hpw::entity_mgr->clear();
  console.print("all entities erased");
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
  MAKE_CMD (
    "kill",
    "kill <uid> - kill entity by entity-uid."
    "If uid not defined, kill last spawned",
    &kill, &kill_matches )
  MAKE_CMD (
    "clear_entities",
    "clear_entities - erase all entities from game",
    &clear_entities, {} )
    
  #undef MAKE_CMD
} // cmd_entity_init
