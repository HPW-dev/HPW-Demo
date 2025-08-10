#pragma once
#include "util/math/num-types.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/image-fwd.hpp"

/** Создаёт шум похожий на шум Перлена
* @param dst куда рисуем
* @param OFFSET смещение узора
* @param STEP_START начальный уровень детализации
* @param STEP_MUL как влиять на шаг увеличения детализации
* @param AMPLIFY огрубляет перепады высот
* @param SCALE увеличивает узор
* @param HARMONICS чем больше, тем детальнее */
void noise_2d(Image& dst, const Vec OFFSET={}, const int HARMONICS=5, const real STEP_START=3,
  const real STEP_MUL=3, const real AMPLIFY=0.333, const real SCALE=1);
