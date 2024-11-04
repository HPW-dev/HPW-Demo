#pragma once
#include "graphic/effect/light.hpp"

enum class Motion_blur_mode {autoopt = 0, disabled, enabled};
enum class Blur_mode {autoopt = 0, low, high};

namespace graphic {

// common:
inline bool draw_border        {true}; // показывать рамку на весь экране
inline bool show_mouse_cursour {};     // показывать курсор мыши в окне игры
inline bool start_focused      {};     // запускает окно сразу поверх других
inline bool cpu_safe           {};     // при VSync снижает нагрузку на CPU, но -кадры
inline bool fullscreen         {};     // на весь экран
inline bool double_buffering   {true}; // двойная буфферизация
inline bool enable_render      {true}; // включение отображения графики (общее)
inline double gamma            {1};    // уровень гаммы
inline Delta_time effect_state {};     // используется в графических алгоритмах (как time переменнная)

// interpolation:
inline Delta_time lerp_alpha     {};     // для интерполяции движение при рендерере
inline bool enable_motion_interp {true}; // плавное движение

// frameskip:
inline uint frame_skip      {3};    // сколько кадров пропускать
inline bool auto_frame_skip {true}; // пропускать кадры при тормозах игры

// autoopt:
constx Delta_time default_autoopt_timeout {2.5};
inline Delta_time autoopt_timeout_max     {default_autoopt_timeout}; // сколько секунд не сбрасывать render_lag
inline Delta_time autoopt_timeout         {}; // сколько секунд не сбрасывать render_lag
inline bool render_lag                    {}; // true, когда рендер лагает

// blur:
inline real insert_blured_traveled_limit   {10}; // если объект проходит меньшее растояние, то не включать блюр
inline real max_motion_blur_quality_reduct {16}; // как сильно снижать качество блюра при автооптимизации
inline real motion_blur_quality_mul               {1};  // качество размытия (меньше - красивее, больше - быстрее)
inline Motion_blur_mode motion_blur_mode   {};   // качество размытия движения
inline Blur_mode blur_mode                 {};   // качество размытия изображения

// effects:
inline bool blink_particles                {true}; // включает мигание спрайтов при render_lag
inline bool enable_light                   {true}; // отображает вспышки
inline bool enable_heat_distort            {};     // отображает искажение воздуха
inline bool disable_heat_distort_while_lag {true}; // выключать искажение воздуха при лагах
inline Light_quality light_quality         {Light_quality::medium}; // качество вcпышки

} // graphic ns
