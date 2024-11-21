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
// загрузить звуки в звуковую систему
void load_sounds();

// рисует нажатые игровые клавиши
void draw_controls(Image& dst);
// вычисляет контрольные суммы по экзешнику и data.zip
void init_validation_info();
// ставит случайную палитру
void set_random_palette();
