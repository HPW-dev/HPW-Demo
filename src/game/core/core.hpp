#pragma once
///@file управление движком, контроль dt

namespace hpw {
inline int target_ups = 240; /// updates per sec.
inline std::size_t cur_upf {}; /// апдейтов за кадр
inline double target_update_time = 1.0 / target_ups;
inline double cur_ups {}; /// апдейты в секунду
inline double safe_dt {}; /// dt для игры
inline double real_dt {}; /// текущий dt как есть (не юзать в фиксированных апдейтах)
inline double update_time_unsafe {}; /// время обновления в игре
inline unsigned game_updates_safe {}; /// столько апдейтов случилось с начала сцены игры
} // hpw ns