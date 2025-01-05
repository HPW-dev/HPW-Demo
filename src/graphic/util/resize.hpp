#pragma once
#include "util/macro.hpp"
#include "util/str.hpp"
#include "graphic/image/color.hpp"

class Image;
class Sprite;

// содержит блок из 5/9 соседних пикселей
struct Pack9 {
  Pal8 data[9] {};
  uint size {}; // может быть либо 5, либо 9
};

enum class Color_compute {
  most_common = 0,
  max,
  min,
  average
};

enum class Color_get_pattern {
  cross = 0,
  box,
};

// увеличивает картинку в 2 раза
void zoom_x2(Image& dst);
// увеличивает спрайт в 2 раза
void zoom_x2(Sprite& dst);
// увеличивает картинку в 3 раза
void zoom_x3(Image& dst);
// увеличивает картинку в 4 раза
void zoom_x4(Image& dst);
// увеличивает спрайт в 4 раза
void zoom_x4(Sprite& dst);
// увеличивает картинку в 8 раз
void zoom_x8(Image& dst);
// увеличивает спрайт в 8 раз
void zoom_x8(Sprite& dst);
// увеличивает спрайт в 3 раза
Sprite pixel_upscale_x3(cr<Sprite> src);
// увеличивает картинку в 3 раза
Image pixel_upscale_x3(cr<Image> src);
// уменьшает спрайт в 3 раза
Sprite pixel_downscale_x3(cr<Sprite> src,
  Color_get_pattern cgp = Color_get_pattern::cross,
  Color_compute ccf = Color_compute::most_common);
// уменьшает картинку в 3 раза
Image pixel_downscale_x3(cr<Image> src,
  Color_get_pattern cgp = Color_get_pattern::cross,
  Color_compute ccf = Color_compute::most_common);
// паттерн выборки пикселей крест
Pack9 color_get_cross(cr<Image> src, int x, int y);
// паттерн выборки пикселей квадрат
Pack9 color_get_box(cr<Image> src, int x, int y);
// возвращает самый частый цвет
Pal8 most_common_col(const Pack9 colors);
// возвращает самый яркий цвет
Pal8 max_col(const Pack9 colors);
// возвращает самый тёмный цвет
Pal8 min_col(const Pack9 colors);
// среднее между цветами
Pal8 average_col(const Pack9 colors);

// Color_compute -> Str
Str convert(Color_compute ccf);
// Color_get_pattern -> Str
Str convert(Color_get_pattern cgp);
// Str -> Color_compute
Color_compute convert_to_ccf(cr<Str> name);
// Str -> Color_get_pattern
Color_get_pattern convert_to_cgp(cr<Str> name);
// билинейная интерполяция
[[nodiscard]] Image resize_neighbor(cr<Image> src, const uint NEW_SIZE_X, const uint NEW_SIZE_Y);
[[nodiscard]] Image resize_bilinear(cr<Image> src, const uint NEW_SIZE_X, const uint NEW_SIZE_Y);
