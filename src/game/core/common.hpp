#pragma once
///@file общие игровые переменные
#include "util/str.hpp"

namespace hpw {
  
inline int argc {}; /// число аргументов с мейна
inline char** argv {}; /// аргументы с мейна
inline Str cur_dir {}; /// корневая директория
inline bool any_key_pressed {false}; /// факта нажатия нажатия любой клавиши
inline bool shmup_mode {true}; /// традиционный shoot'em-up режим
inline Str exe_sha256 {}; /// чексумма для экзешника игры
inline Str data_sha256 {}; /// чексумма для данных игры

} // hpw ns
