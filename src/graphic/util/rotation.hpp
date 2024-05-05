#pragma once
#include "util/math/vec.hpp"

class Image;
class Sprite;

// отзеркалить по вертикали
[[nodiscard]] Image mirror_h(CN<Image> src);
// отзеркалить по вертикали
[[nodiscard]] Sprite mirror_h(CN<Sprite> src);
// отзеркалить по горизонтали
[[nodiscard]] Image mirror_v(CN<Image> src);
// отзеркалить по горизонтали
[[nodiscard]] Sprite mirror_v(CN<Sprite> src);

// нарисовать повёрнутую картинку в dst
void rotate(CN<Image> src, Image& dst, const Vec center,
  const Vec offset, real degree);
// нарисовать повёрнутый спрайт в dst
void rotate(CN<Sprite> src, Sprite &dst, const Vec center,
  const Vec offset, real degree);

// поворот картинки на 90 градусов вправо @param pass число повторейний поворота
[[nodiscard]] Image rotate90(CN<Image> src, uint pass=1);
// поворот спрайта на 90 градусов вправо @param pass число повторейний поворота
[[nodiscard]] Sprite rotate90(CN<Sprite> src, uint pass=1);
