#pragma once
#include "util/math/num-types.hpp"
#include "util/math/vec.hpp"

class Image;

/** Создаёт шум похожий на шум Перлена
* @param dst куда рисуем
* @param OFFSET смещение узора
* @param STEP_START начальный уровень детализации
* @param STEP_MUL как влиять на шаг увеличения детализации
* @param AMPLIFY огрубляет перепады высот
* @param SCALE увеличивает узор
* @param HARMONICS чем больше, тем детальнее */
void noise_2d(Image& dst, const Vec OFFSET={}, const int HARMONICS=8, const real STEP_START=6,
  const real STEP_MUL=1.5, const real AMPLIFY=0.5, const real SCALE=3);
