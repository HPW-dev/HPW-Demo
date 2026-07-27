#pragma once
/** @file общие игровые переменные */
#include "util/str.hpp"
#include "util/platform.hpp"

namespace hpw {

inline int argc {}; // число аргументов с мейна
inline char** argv {}; // аргументы с мейна
inline Str cur_dir {}; // корневая директория
inline bool any_key_pressed {}; // факт нажатия нажатия любой клавиши
inline bool shmup_mode {true}; // традиционный shoot'em-up режим
inline Str exe_sha256 {}; // чексумма для экзешника игры
inline Str data_sha256 {}; // чексумма для данных игры
inline bool need_tutorial {true}; // играя первый раз, предлагать туториал первым в списке
inline bool first_start {};
inline bool save_last_replay {}; // сохранить последний реплей в именной файл?
inline bool sound_mgr_init_error {}; // true, если не удалось инициализировать звуковую систему
inline bool multiple_apps {}; // true - два и более окна с игрой запущено
inline bool rnd_pal_after_death {}; // true - менять палитру при проигрыше
inline bool collider_autoopt {true}; // перебирать разные варианты детекторов коллизий при лагах
inline Priority process_priority {Priority::normal};

inline Str game_dir = "./../"; // корневая папка игры
inline Str screenshots_path = game_dir + "screenshots/";
inline Str replays_path = game_dir + "replays/";
inline Str data_path = game_dir + "data.zip";
inline Str config_dir = game_dir + "config/"; // папка с нофигами
inline Str config_path = config_dir + "config.yml"; // файл настроек
inline Str nickname_path = config_dir + "nickname.dat"; // файл с никнеймом
inline Str avatar_path = config_dir + "miniature.dat"; // файл с миниатюркой игрока
inline Str log_file_path = game_dir + "log.txt";
inline Str plugin_path = game_dir + "plugin/";
inline Str os_resources_dir = game_dir + "../data/"; // отсюда брать файлы, если их нет в data.zip

} // hpw ns
