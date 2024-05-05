#pragma once

class Image;

// блюр через усреднение
void adaptive_blur(Image& dst, int window_sz=1);
// блюр через усреднение с жирным красным размытием
void adaptive_blur_fat_red(Image& dst, int window_sz=1);
// быстрое размытие
void blur_fast(Image& dst, int window_sz=1);
