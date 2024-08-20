#pragma once
#include "ability.hpp"
#include "ability-id.hpp"
#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"

Str get_name(Ability_id id);
Ability_id get_id(cr<Str> name);
Strs get_ability_names();
// выделяет память под разные классы способностей
Unique<Ability> make_ability(Ability_id id, uint lvl=1);
