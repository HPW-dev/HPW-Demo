#include <cassert>
#include <ranges>
#include <algorithm>
#include "cmd-entity.hpp"
#include "cmd-util.hpp"
#include "game/entity/collidable.hpp"
#include "game/core/entities.hpp"
#include "game/util/game-archive.hpp"
#include "game/util/game-util.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/file/yaml.hpp"
#include "util/math/random.hpp"

namespace {
Uid g_last_uid {}; // последний соспавненный объект
}

inline Uid get_uid(CN<Str> str) {
  return_if (str_tolower(str) == "u", ::g_last_uid);
  return s2n<Uid>(str);
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
  ::g_last_uid = entity->uid;
  console.print("entity \"" + entity_name
    + "\" spawned at {" + n2s(pos.x, 2) + ", "
    + n2s(pos.y, 2) + "} uid: " + n2s(::g_last_uid));
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
  iferror(args.size() < 2, "недостаточно параметров");
  cnauto entity_uid = get_uid(args[1]);
  cnauto entities = hpw::entity_mgr->get_entities();
  cauto it = std::find_if(entities.begin(), entities.end(),
    [&](CN<Entitys::value_type> entity) {
      return entity->uid == entity_uid;
    }
  );

  if (it != entities.end()) {
    cnauto entity = it->get();
    entity->kill();
    console.print("killed entity \"" + entity->name()
      + "\" uid: " + n2s(entity_uid));
  } else {
    error("не удалось убить объект с UID = " << n2s(entity_uid));
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

void print_lives(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  cauto lives = hpw::entity_mgr->lives();
  console.print("активных объектов: " + n2s(lives));
}

void make_copy(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "недостаточно параметров");

  // найти кого копируем
  cauto uid = get_uid(args[1]);
  cauto src = hpw::entity_mgr->find(uid);
  iferror(!src, "объект uid=" << uid << " не найден");
  // задать позицию копии
  Vec pos;
  if (args.size() >= 4) {
    cauto x = s2n<real>(args[2]);
    cauto y = s2n<real>(args[3]);
    pos = Vec(x, y);
  } else {
    pos = rnd_screen_pos_safe();
  }
  // скопировать
  auto dst = hpw::entity_mgr->make(src, src->name(), {});
  assert(dst);
  dst->status = src->status;
  dst->phys = src->phys;
  dst->phys.set_pos(pos);

  console.print("объект " + src->name()
    + " скопирован на координаты {"
    + n2s(pos.x, 2) + ", " + n2s(pos.y, 2) + "} UID="
    + n2s(dst->uid));
}

void teleport(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "недостаточно параметров");

  // найти кого телепортим
  cauto uid = get_uid(args[1]);
  cauto ent = hpw::entity_mgr->find(uid);
  iferror(!ent, "объект uid=" << uid << " не найден");
  // задать позицию для телепорта
  Vec pos;
  if (args.size() >= 4) {
    cauto x = s2n<real>(args[2]);
    cauto y = s2n<real>(args[3]);
    pos = Vec(x, y);
  } else {
    pos = rnd_screen_pos_safe();
  }
  // перенестись
  ent->phys.set_pos(pos);

  console.print("объект " + ent->name()
    + " перемещён на новое место {"
    + n2s(pos.x, 2) + ", " + n2s(pos.y, 2) + "}");
}

void entity_hp(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "недостаточно параметров");

  // find raw entity
  cauto uid = get_uid(args[1]);
  auto ent = hpw::entity_mgr->find(uid);
  iferror(!ent, "объект uid=" << uid << " не найден");

  // cast to collidable
  iferror(!ent->status.collidable, "у объекта "
    << ent->name() << " нету параметра HP");
  auto collidable = ptr2ptr<Collidable*>(ent);
  cauto hp = collidable->get_hp();

  // показать хп
  if (args.size() == 2) {
    console.print("у объекта \"" + collidable->name() + "\" HP = " + n2s(hp));
    return;
  }

  // задать хп
  cauto new_hp = s2n<hp_t>(args.at(2));
  collidable->set_hp(new_hp);
  console.print("объекту \"" + collidable->name() + "\" назначено "
    + n2s(new_hp) + " HP");
}

void entity_dmg(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "недостаточно параметров");

  // find raw entity
  cauto uid = get_uid(args[1]);
  auto ent = hpw::entity_mgr->find(uid);
  iferror(!ent, "объект uid=" << uid << " не найден");

  // cast to collidable
  iferror(!ent->status.collidable, "у объекта "
    << ent->name() << " нету параметра DMG");
  auto collidable = ptr2ptr<Collidable*>(ent);
  cauto dmg = collidable->get_dmg();

  // показать дамаг
  if (args.size() == 2) {
    console.print("у объекта \"" + collidable->name() + "\" DMG = " + n2s(dmg));
    return;
  }

  // задать дамаг
  cauto new_dmg = s2n<hp_t>(args.at(2));
  collidable->set_dmg(new_dmg);
  console.print("объекту \"" + collidable->name() + "\" назначено "
    + n2s(new_dmg) + " DMG");
}

void entity_deg(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "недостаточно параметров");
  cauto uid = get_uid(args[1]);
  cauto deg = (args.size() >= 3 ? s2n<real>(args[2]) : rndr(0, 360));
  auto ent = hpw::entity_mgr->find(uid);
  iferror(!ent, "объект uid=" << uid << " не найден");
  ent->phys.set_deg(deg);
  console.print("объекту \"" + ent->name() + "\" назначен угол "
    + n2s(deg, 2) + " градусов");
}

void entity_force(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 3, "недостаточно параметров");
  cauto uid = get_uid(args[1]);
  cauto force = s2n<real>(args[2]);
  auto ent = hpw::entity_mgr->find(uid);
  iferror(!ent, "объект uid=" << uid << " не найден");
  ent->phys.set_force(force);
  console.print("объекту \"" + ent->name() + "\" назначено сопротивление "
    + n2s(force, 2));
}

struct Flag_struct {
  using Set = std::function<void (Enity_status&, bool)>;
  using Get = std::function<bool (Enity_status)>;
  Str name {};
  Set set {};
  Get get {};
};

namespace {
static Vector<Flag_struct> g_entity_flags {
  #define MAKE_FLAG(FLAG) Flag_struct { \
    .name=#FLAG, \
    .set=[](Enity_status& flag, bool val){ flag.FLAG = val; }, \
    .get=[](Enity_status flag) { return flag.FLAG; } \
  },
  MAKE_FLAG (live)
  MAKE_FLAG (collidable)
  MAKE_FLAG (collided)
  MAKE_FLAG (killed)
  MAKE_FLAG (kill_by_end_anim)
  MAKE_FLAG (kill_by_end_frame)
  MAKE_FLAG (kill_by_timeout)
  MAKE_FLAG (is_bullet)
  MAKE_FLAG (is_enemy)
  MAKE_FLAG (is_player)
  MAKE_FLAG (end_anim)
  MAKE_FLAG (end_frame)
  MAKE_FLAG (layer_up)
  MAKE_FLAG (rnd_frame)
  MAKE_FLAG (rnd_deg)
  MAKE_FLAG (rnd_deg_evr_frame)
  MAKE_FLAG (stop_anim)
  MAKE_FLAG (ignore_player)
  MAKE_FLAG (ignore_master)
  MAKE_FLAG (ignore_bullet)
  MAKE_FLAG (ignore_self_type)
  MAKE_FLAG (ignore_bound)
  MAKE_FLAG (fixed_deg)
  MAKE_FLAG (return_back)
  MAKE_FLAG (goto_prev_frame)
  MAKE_FLAG (no_motion_interp)
  MAKE_FLAG (ignore_scatter)
  MAKE_FLAG (ignore_enemy)
  MAKE_FLAG (disable_contour)
  MAKE_FLAG (disable_heat_distort)
  MAKE_FLAG (disable_light)
  MAKE_FLAG (no_restart_anim)
  MAKE_FLAG (disable_render)
  MAKE_FLAG (no_sound)
  MAKE_FLAG (disable_motion)
  #undef MAKE_FLAG
}; // Entity_flags
} // empty ns

void print_collided(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  cnauto entities = hpw::entity_mgr->get_entities();
  uint total {};
  for (cnauto ent: entities)
    total += ent->status.collided;
  console.print("столкновений " + n2s(total));
}

void print_flags(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 2, "недостаточно параметров");
  cauto uid = get_uid(args[1]);
  auto ent = hpw::entity_mgr->find(uid);
  iferror(!ent, "объект uid=" << uid << " не найден");
  cauto flags = ent->status;
  console.print("флаги объекта \"" + ent->name() + "\":");
  for (cnauto entity_flag: ::g_entity_flags)
    if (entity_flag.get(flags))
      console.print("* " + entity_flag.name);
}

void set_flag(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  iferror(args.size() < 4, "недостаточно параметров");
  cauto uid = get_uid(args[1]);
  cnauto flag_name = args[2];
  cauto yesno = bool(args[3] == "0" ? false : true);

  auto ent = hpw::entity_mgr->find(uid);
  iferror(!ent, "объект uid=" << uid << " не найден");

  auto finded_flag = std::find_if (
    ::g_entity_flags.begin(), ::g_entity_flags.end(),
    [&](CN<decltype(::g_entity_flags)::value_type> it)
      { return it.name == flag_name; }
  );

  iferror(finded_flag == ::g_entity_flags.end(),
    "не найден флаг с названием \"" + flag_name + "\"");

  finded_flag->set(ent->status, yesno);
  console.print("флаг " + flag_name + " объекта \""
    + ent->name() + "\" = " + n2s(yesno));
}

Strs set_flag_matches(Cmd_maker& ctx, Cmd& console, CN<Strs> args) {
  return_if(args.size() < 2, Strs{});
  cauto cmd_name = ctx.name();
  cauto uid = args.at(1);
  Strs ret;

  if (args.size() == 3) {
    cauto flag_name = args.at(2);
    cauto name_filter = [&](CN<decltype(::g_entity_flags)::value_type> it)
      { return it.name.find(flag_name) == 0; };
    for (cnauto flag: ::g_entity_flags | std::views::filter(name_filter))
      ret.push_back(cmd_name + ' ' + uid + ' ' + flag.name + ' ');
    return ret;
  }

  for (cnauto flag: ::g_entity_flags)
    ret.push_back(cmd_name + ' ' + uid + ' ' + flag.name + ' ');
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
  MAKE_CMD (
    "kill",
    "kill <uid> - kill entity by entity-uid."
    "If uid not defined, kill last spawned",
    &kill, &kill_matches )
  MAKE_CMD (
    "clear_entities",
    "clear_entities - erase all entities from game",
    &clear_entities, {} )
  MAKE_CMD (
    "lives",
    "lives - показать все живые объекты",
    &print_lives, {} )
  MAKE_CMD (
    "tp",
    "tp <uid> <x/y> - переносит объект на новое место",
    &teleport, {} )
  MAKE_CMD (
    "hp",
    "hp <uid> <val> - назначает жизни объекту. "
    "Без парамерта показывает сколько жизней",
    &entity_hp, {} )
  MAKE_CMD (
    "dmg",
    "dmg <uid> <val> - назначает урон объекту",
    &entity_dmg, {} )
  MAKE_CMD (
    "deg",
    "deg <uid> <degree> - назначает угол поворота объекту",
    &entity_deg, {} )
  MAKE_CMD (
    "copy",
    "copy <uid> <x/y> - создаёт компию объекта."
    "Без парамерта <x/y> создаёт в любом месте",
    &make_copy, {} )
  MAKE_CMD (
    "force",
    "force <uid> <velue> - настраивает сопротивление объекта",
    &entity_force, {} )
  MAKE_CMD (
    "flags",
    "flags <uid> - показывает флаги объекта",
    &print_flags, {} )
  MAKE_CMD (
    "flag",
    "flag <uid> <flag_name> <1/0> - настраивает конкретные флаги объекта",
    &set_flag, &set_flag_matches )
  MAKE_CMD (
    "collided",
    "показывает число столкнувшихся объектов",
    &print_collided, {} )
    
  #undef MAKE_CMD
} // cmd_entity_init
