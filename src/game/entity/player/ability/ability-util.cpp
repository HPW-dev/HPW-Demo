#include <ranges>
#include "ability-util.hpp"
#include "util/vector-types.hpp"
#include "util/error.hpp"

struct Id_and_name {
  Ability_id id {};
  Str name {};
};

static const Vector<Id_and_name> g_id_and_names {
  {Ability_id::invise, "invisiblity"},
  {Ability_id::powershoot, "powershoot"},
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
