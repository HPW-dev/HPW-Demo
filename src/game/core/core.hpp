#pragma once
#include "util/math/num-types.hpp"

namespace hpw {
inline int target_ups = 240; // updates per sec.
inline std::size_t cur_upf {}; // апдейтов за кадр
inline Delta_time target_update_time = 1.0 / target_ups;
inline Delta_time cur_ups {}; // апдейты в секунду
inline Delta_time safe_dt {}; // dt для игры
inline Delta_time real_dt {}; // текущий dt как есть (не юзать в фиксированных апдейтах)
inline Delta_time tick_time {}; // время одного тика игры
inline uint game_ticks {}; // столько апдейтов случилось с начала сцены игры
inline uint global_ticks {}; // столько апдейтов случилось со старта игры
} // hpw ns