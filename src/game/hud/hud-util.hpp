#pragma once
#include "util/str.hpp"
#include "util/mem-types.hpp"
#include "util/macro.hpp"

class Hud;

// создаёт интерфейс по имени
Unique<Hud> make_hud(cr<Str> name);
// узнать какие есть имена интерфейсов
Strs hud_names();
