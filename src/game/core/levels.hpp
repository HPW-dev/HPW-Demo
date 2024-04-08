#pragma once
#include "util/mem-types.hpp"
#include "util/str.hpp"

class Level_mgr;

namespace hpw {
inline Shared<Level_mgr> level_mgr {}; // управление уровнями
inline Str first_level_name {"undefined first level"}; // уровень с которого начали игру
inline Str last_level_name {"undefined last level"}; // последний уровень, на котором играли
}
