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

void load_fonts();
void load_animations();
void load_resources();

Str get_random_replay_name();
// рисует нажатые игровые клавиши
void draw_controls(Image& dst);
// вычисляет контрольные суммы по экзешнику и data.zip
void init_validation_info();
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
