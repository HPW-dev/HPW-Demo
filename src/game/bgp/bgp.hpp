#pragma once
#include "util/macro.hpp"
#include "util/str.hpp"
#include "graphic/image/image-fwd.hpp"

// ссыль на функцию рисующую фон меню
using bgp_pf = void (*)(Image& dst, const int bg_state);

// получить фоновой узор по имени
[[nodiscard]] bgp_pf get_bgp(cr<Str> name);
// узнать какие есть названия фонов
[[nodiscard]] Strs get_bgp_names();
// ставить случайный фон
void randomize_menu_bgp();

namespace hpw {
inline Str menu_bgp_name {}; // если ничего не задано, будет случайным
inline bgp_pf menu_bgp {}; // текущий фон меню
inline bool autoswith_bgp {true};
}
