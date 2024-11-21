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

// загрузить шрифты игры
void load_fonts();
// загрузить все анимации
void load_animations();
// загрузить все иговые ресурсы
void load_resources();
// загрузить звуки в звуковую систему
void load_sounds();

Str get_random_replay_name();
// рисует нажатые игровые клавиши
void draw_controls(Image& dst);
// вычисляет контрольные суммы по экзешнику и data.zip
void init_validation_info();
// сохраняет все текстуры в папку в виде атласной текстуры
void save_all_sprites(cr<Str> save_dir, const int MX=512, const int MY=512);
// размер всех спрайтов в банке
std::size_t sizeof_all_sprites();
// ставит случайную палитру
void set_random_palette();
