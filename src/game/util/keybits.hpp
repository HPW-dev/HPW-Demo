#pragma once
#include <array>
#include "util/unicode.hpp"
#include "util/vector-types.hpp"
/// @file абстракция нажатых клавишь

namespace hpw {

  enum class keycode {
    error = 0,
    up,
    down,
    left,
    right,
    focus,       /// замедление вращения и концетрация огня
    shoot,       /// выстрел вперёд
    bomb,        /// заюзать белого
    mode,        /// смена режима чего-либо
    escape,
    enable,      /// enter

    reset,       /// дебажный перезапуск
    fast_forward,/// быстрая перемотка
    debug,       /// режим отладки
    fps,         /// показ фпс
    fulscrn,     /// открыть на весь экран
    screenshot,  /// сделать скриншот

    max_code // всегда держи это снизу списка
  }; // Keycode

  struct Key {
    bool cur {};
    bool prev {};
  };

  /// таблица с текущими и прошлыми нажатыми клавишами
  inline std::array<Key, scast<std::size_t>(keycode::max_code)> keys {};

} // npw ns

//! коды и системные имена клавишь
struct Keys_info {
  struct Item {
    hpw::keycode hpw_key {}; /// игровой код клавиши
    utf32 name {}; /// системное имя клавиши
    int scancode {}; /// сканкод клавиши с клавиатуры
  };
  Vector<Item> keys {};

  CP<Item> find(hpw::keycode hpw_key) const;
}; // Keys_info

namespace hpw {
  /// таблица соответствия игровых клавишь и системных сканкодов
  inline Keys_info keys_info {};
}

/// установить код нажатой кнопки
void press(hpw::keycode code);
/// отпустить кнопку
void release(hpw::keycode code);
/// сбросить текущие нажатые клавиши
void clear_cur_keys();
/// перенести текущие нажатия в предыдущие
void keys_cur_to_prev();
/// узнать что кнопка нажата
bool is_pressed(hpw::keycode code);
/// узнать что кнопка нажата один раз
bool is_pressed_once(hpw::keycode code);
/// узнать что любая из кнопок нажата
bool is_any_key_pressed();
