#pragma once
#include "graphic/effect/light.hpp"

namespace graphic {

inline real max_motion_blur_quality_reduct {16}; // как сильно снижать качество блюра при автооптимизации
constx Delta_time default_autoopt_timeout  {2.5};
inline Delta_time autoopt_timeout_max = default_autoopt_timeout; // сколько секунд не сбрасывать render_lag
inline Delta_time autoopt_timeout {}; // сколько секунд не сбрасывать render_lag
inline real insert_blured_traveled_limit {10}; // если объект проходит меньшее растояние, то не включать блюр
inline real blur_quality_mul     {1}; // качество размытия (меньше - красивее, больше - быстрее)
inline Delta_time lerp_alpha     {}; // для интерполяции движение при рендерере
inline Delta_time effect_state   {}; // используется в графических алгоритмах (как time переменнная)
inline bool render_lag           {}; // true, когда рендер лагает
inline bool enable_render        {true}; // включение отображения графики (общее)
inline bool blink_particles      {true}; // включает мигание спрайтов при render_lag
inline bool blink_motion_blur    {true}; // включает мигание при размытии движения
inline bool motion_blur_quality_reduct {true}; // уменьшить качество размытия при render_lag
inline bool enable_motion_blur   {true}; // включение размытия движения
inline bool enable_motion_interp {true}; // плавное движение
inline bool cpu_safe             {}; // при VSync снижает нагрузку на CPU, но -кадры
inline bool fullscreen           {}; // на весь экран
inline bool double_buffering     {true}; // двойная буфферизация
inline bool draw_border          {true}; // показывать рамку на весь экране
inline bool show_mouse_cursour   {}; // показывать курсор мыши в окне игры
inline bool start_focused        {}; // запускает окно сразу поверх других
inline bool enable_light         {true}; // отображает вспышки
inline bool enable_heat_distort  {}; // отображает искажение воздуха
inline bool disable_heat_distort_while_lag {true}; // выключать искажение воздуха при лагах
inline Light_quality light_quality {Light_quality::medium}; // качество вcпышки
inline uint frame_skip           {2}; // сколько кадров пропускать
inline bool auto_frame_skip      {true}; // пропускать кадры при тормозах игры
inline bool fast_blur            {true}; // использовать оптимизированное размытие
inline double gamma              {1}; // уровень гаммы

} // graphic ns
