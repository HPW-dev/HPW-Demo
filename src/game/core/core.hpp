#pragma once
#include "util/math/num-types.hpp"

namespace hpw {
inline int target_ups = 240; // updates per sec.
inline std::size_t cur_upf {}; // апдейтов за кадр
inline Delta_time target_tick_time = 1.0 / target_ups;
inline Delta_time cur_ups {}; // апдейты в секунду
inline Delta_time safe_dt {}; // dt для игры
inline Delta_time real_dt {}; // текущий dt как есть (не юзать в фиксированных апдейтах)
inline Delta_time tick_time {}; // время одного тика игры
inline Delta_time tick_start_time {}; // когда начался текущий тик
inline Delta_time tick_start_time_prev {}; // время начала предыдущего тика
inline Delta_time tick_end_time {}; // когда завершился последний тик
inline Delta_time tick_time_accum {}; // сумма пройденного времени между апдейтами
inline Delta_time soft_draw_start_time {}; // время начала софтверной отрисовки кадра
inline uint game_ticks {}; // столько апдейтов случилось с начала сцены игры
inline uint global_ticks {}; // столько апдейтов случилось со старта игры
} // hpw ns