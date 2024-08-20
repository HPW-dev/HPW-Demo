#pragma once
// @file функции для рисования на картинке
#include <cassert>
#include "util/error.hpp"
#include "util/math/vec.hpp"
#include "util/math/rect.hpp"
#include "util/math/num-types.hpp"
#include "graphic/image/color-blend.hpp"
#include "graphic/image/mode-get.hpp"

struct Polygon;
struct Pal8;
class Image;
class Sprite;

// ищет пиксель-бленд функцию по имени
[[nodiscard]] blend_pf find_blend_f(cr<Str> name);

// рисует полигон
void draw_polygon(Image& dst, const Vec pos, cr<Polygon> poly,
  const Pal8 color);
// смешивает две картинки
void blend(Image& dst, cr<Image> src, const Vec pos, real alpha, 
  blend_pf bf=&blend_past, int optional=0);
// смешивает картинку и спрайт
void blend(Image& dst, cr<Sprite> src, const Vec pos, real alpha,
  blend_pf bf=&blend_past, int optional=0);

// рисует src увеличенным в 2 раза на dst
void insert_x2(Image& dst, cr<Image> src, Vec pos);

// обрезать лишнее в спрайте
[[nodiscard]] Sprite optimize_size(cr<Sprite> src, Vec& offset);

// вырезание области без проверко границ
[[nodiscard]] Image fast_cut(cr<Image> src, int sx, int sy, int mx, int my);

// вырезание области (безопасное)
[[nodiscard]] Image cut(cr<Image> src, cr<Rect> rect,
  Image_get mode=Image_get::MIRROR);

// найти центр спрайта
[[nodiscard]] Vec center_point(cr<Sprite> src);
// найти центр картинки
[[nodiscard]] Vec center_point(cr<Image> src);
// чтобы вставить картинку dst в центр картинки src
[[nodiscard]] Vec center_point(cr<Image> src, cr<Image> dst);
// чтобы вставить спрайт dst в центр картинки src
[[nodiscard]] Vec center_point(cr<Image> src, cr<Sprite> dst);
[[nodiscard]] Vec center_point(const Vec src, const Vec dst);

// получить случайный цвет
[[nodiscard]] Pal8 rand_color_stable(bool red=false);
// получить случайный цвет (не синхронизирован с реплеем и быстрее)
[[nodiscard]] Pal8 rand_color_graphic(bool red=false);

// вставить картинку в картинку
void insert(Image& dst, cr<Image> src, Vec pos, blend_pf bf,
  int optional=0);
// вставить картинку в спрайт
void insert(Image& dst, cr<Sprite> src, Vec pos, blend_pf bf,
  int optional=0);

// вставить картинку в картинку (быстро, без проверок)
void insert_fast(Image& dst, cr<Image> src);

void add_brightness(Image& dst, const Pal8 brightness);
void sub_brightness(Image& dst, const Pal8 brightness);
void apply_invert(Image& dst);
// превратить все цвета в красный
void to_red(Image& dst);
// превратить все оттенки в серый
void to_gray(Image& dst);

// распространение пикселей от цвета color в 4-х направлениях
void expand_color_4(Image& dst, const Pal8 color);
// распространение пикселей от цвета color в 8-и направлениях
void expand_color_8(Image& dst, const Pal8 color);

void insert_blured(Image& dst, cr<Sprite> src, const Vec old_pos,
  const Vec cur_pos, blend_pf bf, Uid uid=0);
// определяет какую область src надо копировать в пределах dst
[[nodiscard]] Rect get_insertion_bound(cr<Image> dst, const Vec pos, cr<Image> src);
// контраст (0 .. 1 .. inf)
void apply_contrast(Image& dst, real contrast);
// меняет яркость картинки
void apply_brightness(Image& dst, const int val);
// рисует плавную кривую (сплайн)
void draw_spline(Image& dst, const Vec a, const Vec b,
  const Vec c, const Vec d, const Pal8 color,
  const blend_pf bf=blend_past, const uint quality=30);

// рисует линию между двумя точками
void draw_line(Image& dst, Vec _p1, const Vec _p2,
  const Pal8 color, const blend_pf bf);