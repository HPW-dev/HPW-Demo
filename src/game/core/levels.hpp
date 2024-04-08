#pragma once
#include "util/mem-types.hpp"
#include "util/str.hpp"

class Level_mgr;

namespace hpw {
inline Shared<Level_mgr> level_mgr {}; // управление уровнями
inline bool first_level_is_tutorial {};
}
