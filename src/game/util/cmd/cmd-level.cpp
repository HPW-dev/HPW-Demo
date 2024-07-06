#include <unordered_map>
#include <ranges>
#include "cmd-level.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "game/core/levels.hpp"
#include "game/core/entities.hpp"
#include "game/core/sounds.hpp"
#include "game/core/canvas.hpp"
#include "game/util/game-util.hpp"
#include "game/util/game-archive.hpp"
#include "game/level/level-empty.hpp"
#include "game/level/level-space.hpp"
#include "game/level/level-tutorial.hpp"
#include "game/entity/collider/collider-empty.hpp"
#include "game/entity/collider/collider-simple.hpp"
#include "game/entity/collider/collider-qtree.hpp"
#include "game/entity/collider/collider-experimental.hpp"

// привязка имён уровней к их конструкторам
static std::unordered_map<Str, Level_mgr::Maker> g_game_levels {
  {str_tolower(Str{Level_empty::NAME}), []{ return new_shared<Level_empty>(); }},
  {str_tolower(Str{Level_tutorial::NAME}), []{ return new_shared<Level_tutorial>(); }},
  {str_tolower(Str{Level_space::NAME}), []{ return new_shared<Level_space>(); }},
};

void Cmd_levels::exec(CN<Strs> cmd_and_args) {
  Str list = "Avaliable game levels:\n";
  for (cnauto [key, maker]: g_game_levels)
    list += "- " + key + '\n';
  m_master->print(list);
}

// перезагрузка ресурсов
static inline void reload_resources() {
  hpw::sound_mgr->shutup();
  init_archive();
  hpw::entity_mgr->clear();
  load_animations();
  hpw::entity_mgr->register_types();
}

void Cmd_level::exec(CN<Strs> cmd_and_args) {
  iferror(cmd_and_args.size() < 2, "need more arguments in level command");
  cnauto level_name = str_tolower( cmd_and_args.at(1) );
  cnauto level_maker = g_game_levels.at(level_name);
  iferror(!level_maker, "level_maker is null");
  hpw::level_mgr->set(level_maker);
  m_master->print("Level \"" + level_name + "\" selected");
}

Strs Cmd_level::command_matches(CN<Strs> cmd_and_args) {
  return_if(cmd_and_args.size() > 2, Strs{});

  Strs ret;
  cauto cmd_name = cmd_and_args.at(0);

  // отфильтровать пользоватеьский ввод
  if (cmd_and_args.size() == 2) {
    cnauto level_name = cmd_and_args.at(1);
    cauto name_filter = [&](CN<decltype(g_game_levels)::value_type> it)
      { return it.first.find(level_name) == 0; };
    for (cnauto [name, maker]: g_game_levels | std::views::filter(name_filter))
      ret.push_back(cmd_name + ' ' + name);
    return ret;
  }

  // предложить уровни из списка
  for (cnauto [key, maker]: g_game_levels)
    ret.push_back(cmd_name + ' ' + key);
  return ret;
} // command_matches

void Cmd_restart::exec(CN<Strs> cmd_and_args) {
  reload_resources();
  // рестарт текущего уровня
  cauto level_name = str_tolower( hpw::level_mgr->level_name() );
  cnauto level_maker = g_game_levels.at(level_name);
  iferror(!level_maker, "level_maker is null");
  hpw::level_mgr->set(level_maker);
  m_master->print("Level \"" + level_name + "\" restarted");
}

using Collider_maker = std::function< Shared<Collider> ()>;
static std::unordered_map<Str, Collider_maker> g_colliders {
  {"empty", []{ return new_shared<Collider_empty>(); }},
  {"simple", []{ return new_shared<Collider_simple>(); }},
  {"experimental", []{ return new_shared<Collider_experimental>(); }},
  {"qtree", []{ return new_shared<Collider_qtree>(7, 1, graphic::width, graphic::height); }},
};

void Cmd_collider::exec(CN<Strs> cmd_and_args) {
  iferror(cmd_and_args.size() < 2, "need more parameters for collider command");
  cnauto collider_name = cmd_and_args.at(1);
  cnauto maker = g_colliders.at(collider_name);
  hpw::entity_mgr->set_collider(maker());
  m_master->print("selected \"" + collider_name + "\" collision resolver");
}

Strs Cmd_collider::command_matches(CN<Strs> cmd_and_args) {
  Strs ret;
  cauto cmd_name = cmd_and_args.at(0);
  if (cmd_and_args.size() < 2) {
    // предложить системы из списка
    for (cnauto [collider_name, maker]: g_colliders)
      ret.push_back(cmd_name + ' ' + collider_name);
    return ret;
  }

  // по вводу определить что взять из списка автодополнения
  cauto arg_name = cmd_and_args.at(1); // имя колайдер ресолвера
  cauto name_filter = [&](CN<decltype(g_colliders)::value_type> it)
    { return it.first.find(arg_name) == 0; };
  for (cnauto [collider_name, maker]: g_colliders | std::views::filter(name_filter))
    ret.push_back(cmd_name + ' ' + collider_name);
  return ret;
}
