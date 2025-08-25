#pragma once
#include "game/hud/hud.hpp"
#include "util/mem-types.hpp"
#include "util/str.hpp"

namespace graphic {
constx Str DEFAULT_HUD = "asci";
inline Str cur_hud {DEFAULT_HUD}; // имя выбранного интрфейса
inline Unique<Hud> hud {}; // HUD в игре
}
