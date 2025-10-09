#pragma once
#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/math/num-types.hpp"

namespace hpw {
inline Str bgp_for_menu {}; // если ничего не задано, будет выбран случайно
constx Delta_time BGP_AUTO_SWITCH_DELAY {}; // сколько надо ждать для авто смены фона
inline bool bgp_auto_swith {true}; // автосмена фонового узора
}
