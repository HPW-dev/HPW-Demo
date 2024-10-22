#pragma once
#include "util/macro.hpp"

class Image;

// блюр через усреднение
void adaptive_blur(Image& dst, int window_sz=1);
// блюр через усреднение с жирным красным размытием
void adaptive_blur_fat_red(Image& dst, int window_sz=1);
// быстрое размытие
void blur_fast(Image& dst, int window_sz=1);
// размытие по алгоритму BoxBlur (аккуратные края + для серого изображения)
void boxblur_gray_accurate(Image& dst, cr<Image> src, const int window_sz=1);
// размытие по алгоритму BoxBlur (быстрый вариант)
void boxblur_gray_fast(Image& dst, cr<Image> src, const int window_sz=1);
