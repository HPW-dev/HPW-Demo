#pragma once
/** @file общие игровые переменные */
#include "util/str.hpp"

namespace hpw {

inline int argc {}; // число аргументов с мейна
inline char** argv {}; // аргументы с мейна
inline Str cur_dir {}; // корневая директория
inline bool any_key_pressed {}; // факт нажатия нажатия любой клавиши
inline bool shmup_mode {true}; // традиционный shoot'em-up режим
inline Str exe_sha256 {}; // чексумма для экзешника игры
inline Str data_sha256 {}; // чексумма для данных игры
inline bool need_tutorial {true}; // играя первый раз, предлагать туториал первым в списке
inline bool save_last_replay {}; // сохранить последний реплей в именной файл?
inline bool sound_mgr_init_error {}; // true, если не удалось инициализировать звуковую систему
inline bool multiple_apps {}; // true - два и более окна с игрой запущено

} // hpw ns
