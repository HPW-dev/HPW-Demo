#pragma once
#include "util/macro.hpp"

class Image;

// размытие по алгоритму BoxBlur (аккуратные края + для серого изображения)
void boxblur_gray_accurate(Image& dst, cr<Image> src, const int window_sz=1);
// размытие по алгоритму BoxBlur (быстрый вариант)
void boxblur_gray_fast(Image& dst, cr<Image> src, const int window_sz=1);
// размытие по алгоритму BoxBlur (быстрый вариант; только горизонтальные пиксели)
void boxblur_horizontal_gray_fast(Image& dst, cr<Image> src, const int window_sz=1);
// красивое размытие (для серых изображений; ядро с линейным градиентом)
void blur_gray_accurate(Image& dst, cr<Image> src, const int window_sz=1);
// текущая быстрая реализация размытия
constexpr static const auto blur_fast = &boxblur_horizontal_gray_fast;
// текущая красивая реализация размытия
constexpr static const auto blur_hq = &blur_gray_accurate;
