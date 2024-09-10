#pragma once
#include "util/mem-types.hpp"
#include "util/str.hpp"

class Hud;

namespace graphic {
inline Str cur_hud {"ASCI"}; // имя выбранного интрфейса
inline Unique<Hud> hud {}; // HUD в игре
}
