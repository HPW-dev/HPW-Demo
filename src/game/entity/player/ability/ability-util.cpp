#include <ranges>
#include <cassert>
#include "ability-util.hpp"
#include "ability-dummy.hpp"
#include "util/vector-types.hpp"
#include "util/error.hpp"

struct Id_and_name {
  Ability_id id {};
  Str name {};
};

static const Vector<Id_and_name> g_id_and_names {
  {Ability_id::invise, "invisiblity"},
  {Ability_id::powershoot, "powershoot"},
  {Ability_id::dummy, "dummy"},
};

Str get_name(Ability_id id) {
  cauto find_f = [id](cr<Id_and_name> it)->bool { return it.id == id; };
  cauto ret = std::find_if(g_id_and_names.begin(), g_id_and_names.end(), find_f);
  iferror(ret == g_id_and_names.end(), "не удалось найти имя способности с id = " << scast<int>(id));
  return ret->name;
}

Ability_id get_id(cr<Str> name) {
  cauto find_f = [name](cr<Id_and_name> it)->bool { return it.name == name; };
  cauto ret = std::find_if(g_id_and_names.begin(), g_id_and_names.end(), find_f);
  iferror(ret == g_id_and_names.end(), "не удалось найти id способности с именем \"" << name << "\"");
  return ret->id;
}

Strs get_ability_names() {
  Strs ret;
  for (crauto [_, name]: g_id_and_names)
    ret.push_back(name);
  return ret;
}

Unique<Ability> make_ability(Ability_id id, const uint lvl) {
  assert(lvl > 0);
  Unique<Ability> ret {};

  switch (id) {
    case Ability_id::dummy: ret = new_unique<Ability_dummy>(); break;
    default: error("незарегистрированный id способности (" << scast<int>(id) << ")");
  }

  // докрутить до нужного уровня
  assert(ret);
  cfor (i, lvl - 1)
    ret->on_upgrade();

  return ret;
}
