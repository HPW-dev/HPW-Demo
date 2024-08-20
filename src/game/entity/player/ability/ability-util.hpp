#pragma once
#include "ability-id.hpp"
#include "util/str.hpp"
#include "util/macro.hpp"

Str get_name(Ability_id id);
Ability_id get_id(cr<Str> name);
