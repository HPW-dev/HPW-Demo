#pragma once
#include "game/hud/hud.hpp"
#include "util/mem-types.hpp"
#include "util/str.hpp"

namespace graphic {
inline Str cur_hud {"ASCI"}; // имя выбранного интрфейса
inline Unique<Hud> hud {}; // HUD в игре
}
