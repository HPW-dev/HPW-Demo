#pragma once
#include "util/math/vec.hpp"
#include "resize.hpp"
#include "graphic/image/image-fwd.hpp"

class Sprite;

/** поворачивает картинку и оптимизирует по размеру краёв
* @param src что вращается
* @param degree на какой угол
* @param offset для поворота смещения кадров анимации
* @param rotation_offset для точно подгонки при генерации спрайтов
* @param cgp метод выборки пикселей из исходника при даунскейле
* @param ccf метод определения цвета при даунскейле */
Sprite rotate_and_optimize(
  cr<Sprite> src, real degree, Vec& offset,
  const Vec rotation_offset = {},
  Color_get_pattern cgp = Color_get_pattern::cross,
  Color_compute ccf = Color_compute::most_common
);
