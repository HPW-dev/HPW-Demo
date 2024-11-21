#pragma once
#include "util/macro.hpp"
#include "util/mem-types.hpp"
#include "util/str.hpp"

class Anim;

/** создаёт осветлённую обводку для контуров спрайта
* @details анимация сама сохранится в банк
* @param src с какой анимации взять спрайты
* @param dst как назвать новую анимацию
* @return ссыль получившуюся анимацию */
cr<Shared<Anim>> make_light_mask(cr<Str> src, cr<Str> dst);
