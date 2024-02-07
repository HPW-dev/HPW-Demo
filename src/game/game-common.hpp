#pragma once
///@file общие игровые переменные
#include "game/util/store.hpp"
#include "util/str.hpp"

class Locale;
class Entity_mgr;
class Scene_mgr;
class Level_mgr;
class Anim_mgr;
class Sprite;
class Yaml;

namespace hpw {
  
inline int argc {}; /// число аргументов с мейна
inline char** argv {}; /// аргументы с мейна
inline Str cur_dir {}; /// корневая директория
inline bool any_key_pressed {false}; /// факта нажатия нажатия любой клавиши
inline bool shmup_mode {true}; /// традиционный shoot'em-up режим
inline Str exe_sha256 {}; /// чексумма для экзешника игры
inline Str data_sha256 {}; /// чексумма для данных игры

inline Shared<Yaml> config {}; /// config.yml
inline Shared<Locale> locale {}; /// строки локализации
inline Shared<Store<Sprite>> store_sprite {}; /// все спрайты тут
inline Shared<Store<Locale>> store_locale {}; /// локализованные строки
inline Shared<Entity_mgr> entity_mgr {}; /// управление сущностями игры
inline Shared<Scene_mgr> scene_mgr {}; /// управление сценами
inline Shared<Level_mgr> level_mgr {}; /// управление уровнями
inline Shared<Anim_mgr> anim_mgr {}; /// управление анимациями

} // hpw ns
