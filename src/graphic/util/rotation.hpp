#pragma once
#include "util/math/vec.hpp"
#include "graphic/image/image-fwd.hpp"
class Sprite;

// отзеркалить по вертикали
[[nodiscard]] Image mirror_h(cr<Image> src);
// отзеркалить по вертикали
[[nodiscard]] Sprite mirror_h(cr<Sprite> src);
// отзеркалить по горизонтали
[[nodiscard]] Image mirror_v(cr<Image> src);
// отзеркалить по горизонтали
[[nodiscard]] Sprite mirror_v(cr<Sprite> src);

// нарисовать повёрнутую картинку в dst
void rotate(cr<Image> src, Image& dst, const Vec center,
  const Vec offset, real degree);
// нарисовать повёрнутый спрайт в dst
void rotate(cr<Sprite> src, Sprite &dst, const Vec center,
  const Vec offset, real degree);

/** поворот картинки на 90 градусов вправо
* @param pass число повторейний поворота */
[[nodiscard]] Image rotate_90(cr<Image> src, uint pass=1);
[[nodiscard]] Image rotate_180(cr<Image> src);
[[nodiscard]] Image rotate_270(cr<Image> src);
/** поворот спрайта на 90 градусов вправо
* @param pass число повторейний поворота */
[[nodiscard]] Sprite rotate_90(cr<Sprite> src, uint pass=1);
