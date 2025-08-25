#pragma once
#include "graphic/image/image-fwd.hpp"

bool check_high_blur(); // узнать, можно ли заюзать качественное размытие изображения
// блюр зависящий от настроек графики
void hpw_blur(Image& dst, const Image& src, const int window_sz=1);
