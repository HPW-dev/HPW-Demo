#pragma once
/// @file функции для рисования на картинке
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

/// ищет пиксель-бленд функцию по имени
blend_pf find_blend_f(CN<Str> name);

/// рисует полигон
void draw_polygon(Image& dst, const Vec pos, CN<Polygon> poly, const Pal8 color);
/// смешивает две картинки
void blend(Image& dst, CN<Image> src, const Vec pos, real alpha, blend_pf bf=&blend_past, int optional=0);
/// смешивает картинку и спрайт
void blend(Image& dst, CN<Sprite> src, const Vec pos, real alpha, blend_pf bf=&blend_past, int optional=0);

/// рисует src увеличенным в 2 раза на dst
void insert_x2(Image& dst, CN<Image> src, Vec pos);

/// обрезать лишнее в спрайте
Sprite optimize_size(CN<Sprite> src, Vec& offset);

/// вырезание области без проверко границ
Image fast_cut(CN<Image> src, int sx, int sy, int mx, int my);

/// вырезание области (безопасное)
Image cut(CN<Image> src, CN<Rect> rect, Image_get mode = Image_get::MIRROR);

/// найти центр спрайта
Vec center_point(CN<Sprite> src);
/// найти центр картинки
Vec center_point(CN<Image> src);
/// чтобы вставить картинку dst в центр картинки src
Vec center_point(CN<Image> src, CN<Image> dst);
/// чтобы вставить спрайт dst в центр картинки src
Vec center_point(CN<Image> src, CN<Sprite> dst);
Vec center_point(const Vec src, const Vec dst);

/// получить случайный цвет
Pal8 rand_color_stable(bool red=false);
/// получить случайный цвет (не синхронизирован с реплеем и быстрее)
Pal8 rand_color_graphic(bool red=false);

/// вставить картинку в картинку
void insert(Image& dst, CN<Image> src, Vec pos, blend_pf bf, int optional=0);
/// вставить картинку в спрайт
void insert(Image& dst, CN<Sprite> src, Vec pos, blend_pf bf, int optional=0);

/// вставить картинку в картинку (быстро, без проверок)
void insert_fast(Image& dst, CN<Image> src);

void add_brightness(Image& dst, const Pal8 brightness);
void sub_brightness(Image& dst, const Pal8 brightness);
void apply_invert(Image& dst);
/// превратить все цвета в красный
void to_red(Image& dst);

/// распространение пикселей от цвета color в 4-х направлениях
void expand_color_4(Image& dst, const Pal8 color);
/// распространение пикселей от цвета color в 8-и направлениях
void expand_color_8(Image& dst, const Pal8 color);

void insert_blured(Image& dst, CN<Sprite> src, const Vec old_pos, const Vec cur_pos,
  blend_pf bf, Uid uid=0);
/// определяет какую область src надо копировать в пределах dst
Rect get_insertion_bound(CN<Image> dst, const Vec pos, CN<Image> src);
/// контраст (0 .. 1 .. inf)
void apply_contrast(Image& dst, double contrast);
