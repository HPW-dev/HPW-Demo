#pragma once
#include <functional>

#include "game/core/core-window.hpp"
#include "game/input/keys.hpp"
#include "util/str.hpp"


namespace Port {
  
#define STDFN(FUNC_TYPE, FUNC_NAME) inline std::function<FUNC_TYPE> FUNC_NAME {};

// ---- Input ------------------------------------------------------------------
// Переназначить клавишу
STDFN(void (hpw::keycode), rebind_key)
// Переназначить клавишу по её сканкоду
STDFN(void (hpw::keycode, int), rebind_key_by_scancode)
// сброс настроек клавиатуры
STDFN(void (), reset_keymap)

// ---- Graphic ----------------------------------------------------------------
// V-Sync вкл/выкл
STDFN(void (bool yes_no), toggle_vsync)
// Установка палитры из файла
STDFN(void (cr<Str> path), set_palette)
// Выставить гамму для моника
STDFN(void (double value), set_gamma)
// Растянуть игру на полный экран
STDFN(void (bool yes_no), toggle_fullscreen)
// Двойная буфферизация вкл/выкл
STDFN(void (bool yes_no), toggle_double_buffering)
// Выбирает режим растягивания окна игры
STDFN(void (Resize_mode), set_resize_mode)
// Показ курсора мыши вкл/выкл
STDFN(void (bool yes_no), toggle_mouse_visible)

// ---- Misc -------------------------------------------------------------------
// Вежливо попросить игру завершиться
STDFN(void (), soft_exit)
// Сохранить сниомк экрана
STDFN(void (), make_screenshot)
// TODO перетащить это в подходящее место
STDFN(void (uint), set_target_ups)
// Получить время со старта игры (Сек)
STDFN(double (), get_time)

#undef STDFN

// ---- Основные функции порта -------------------------------------------------
// Для показа ошибки юзеру
void process_error(cr<Str> error);
// Создаёт окно для игры и грузит функции портов
void init(); 
// false, если игровой цикл завершили
[[nodiscard]] bool is_ran();
// Обновить состояния порта, чекнуть физический инпут, посчитать Dt
void update();
// Нарисовать полноэкранную текстуру игры
void render_canvas();
// Задержка игры (V-Sync)
void sync();
// Действия по завершению - закрытие окна, сейв конфигов, освобождение ресурсов
void finalize();

} // Port ns
