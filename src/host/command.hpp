#pragma once
// @file глобальные колбэки
#include <functional>
#include "util/str.hpp"
#include "util/math/num-types.hpp"

enum class Resize_mode;

namespace hpw {

enum class keycode;

// программный выход
inline std::function<void ()> soft_exit {};
// вкл/выкл полный экран
inline std::function<void (bool)> set_fullscreen {};
// вкл/выкл Vsync
inline std::function<void (bool)> set_vsync {};
// вкл/выкл двойную буфферицию
inline std::function<void (bool)> set_double_buffering {};
// выбор режима растягивания
inline std::function<void (Resize_mode)> set_resize_mode {};
// вкл/выкл видимость курсора мыши на окне
inline std::function<void (bool)> set_mouse_cursour_mode {};
// сделать скриншот
inline std::function<void ()> make_screenshot {};
// переназначить клавишу
inline std::function<void (keycode)> rebind_key {};
// переназначить клавишу по её сканкоду
inline std::function<void (keycode, int)> rebind_key_by_scancode {};
// сброс настроек клавиатуры
inline std::function<void ()> reset_keymap {};
// меняет палитру в игре
inline std::function<void (const Str&)> init_palette_from_archive {};
// устанавливает гамму
inline std::function<void (const double)> set_gamma {};
// для вычисления точных временных интервалов
inline std::function<Delta_time ()> get_time {};

} // hpw ns
