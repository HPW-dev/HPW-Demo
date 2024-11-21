#pragma once
#include "game/core/sprites.hpp"
#include "game/core/difficulty.hpp"
#include "util/unicode.hpp"
#include "util/str.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"

class Image;
class Sprite;
class Yaml;
class Locale;

void load_fonts();
void load_locale(cr<Str> user_path={});
void load_animations();
void load_resources();
// безопасное получение локализованной строки
cr<utf32> get_locale_str(cr<Str> key);
Str get_random_replay_name();
// рисует нажатые игровые клавиши
void draw_controls(Image& dst);
// вычисляет контрольные суммы по экзешнику и data.zip
void init_validation_info();
void init_scene_mgr();
// перевести названия уровней сложности
utf32 difficulty_to_str(const Difficulty difficulty);
// сохраняет все текстуры в папку в виде атласной текстуры
void save_all_sprites(cr<Str> save_dir, const int MX=512, const int MY=512);
// размер всех спрайтов в банке
std::size_t sizeof_all_sprites();
// загрузить звуки в звуковую систему
void load_sounds();
// ставит случайную палитру
void set_random_palette();
// сохраняет скриншот в нужную папку с нужным именем
void save_screenshot(cr<Image> image);
// блюр зависящий от настроек графики
void hpw_blur(Image& dst, cr<Image> src, const int window_sz=1);
// узнать, можно ли заюзать качественное размытие изображения
bool check_high_blur();
