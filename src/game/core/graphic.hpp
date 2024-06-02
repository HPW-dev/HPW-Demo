#pragma once
#include "graphic/effect/light.hpp"

namespace graphic {

// как сильно снижать качество блюра при автооптимизации
inline real max_motion_blur_quality_reduct = 16.0;
constx Delta_time default_autoopt_timeout = 2.5;
// сколько секунд не сбрасывать render_lag
inline Delta_time autoopt_timeout_max = default_autoopt_timeout;
// если объект проходит меньшее растояние, то не включать блюр
inline real insert_blured_traveled_limit = 10;
inline real blur_quality_mul = 1.0; // качество размытия (меньше - красивее, больше - быстрее)
inline Delta_time autoopt_timeout {};
inline Delta_time lerp_alpha {}; // для интерполяции движение при рендерере
inline Delta_time effect_state {};
inline bool render_lag          {false}; // true, когда рендер лагает
inline bool enable_render       {true};
inline bool blink_particles     {true}; // включает мигание спрайтов при render_lag
inline bool blink_motion_blur   {true}; // включает мигание при размытии движения
inline bool motion_blur_quality_reduct {true}; // уменьшить качество размытия при render_lag
inline bool enable_motion_blur  {true};
inline bool enable_motion_interp{true}; // плавное движение
inline bool cpu_safe            {false}; // при VSync снижает нагрузку на CPU, но -кадры
inline bool fullscreen          {false};
inline bool double_buffering    {true};
inline bool draw_border         {true}; // показывать рамку на весь экране
inline bool show_mouse_cursour  {false}; // показывать курсор мыши в окне игры
inline bool start_focused       {false}; // запускает окно сразу поверх других
inline bool enable_light        {true}; // отображает вспышки
inline bool enable_heat_distort {false}; // отображает искажение воздуха
inline bool disable_heat_distort_while_lag {true}; // выключать искажение воздуха при лагах
inline Light_quality light_quality {Light_quality::medium}; // качество вcпышки
inline uint frame_skip {2};
inline bool auto_frame_skip {true};
inline double gamma {1.0};

} // graphic ns
