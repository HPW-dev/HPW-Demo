#pragma once
#include "game/core/sprites.hpp"
#include "game/core/difficulty.hpp"
#include "util/unicode.hpp"
#include "util/str.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"

// загрузить шрифты игры
void load_fonts();
// загрузить все анимации
void load_animations();
// загрузить звуки в звуковую систему
void load_sounds();
// вычисляет контрольные суммы по экзешнику и data.zip
void init_validation_info();
