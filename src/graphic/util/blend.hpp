#pragma once
#include "util/macro.hpp"

class Image;

/** накладывает src на dst с картой прозрачности в blend_mask
* @details dst, src и blend_mask должны быть одинаковы по размеру */
void accept_blend_mask_fast(Image& dst, CN<Image> src, CN<Image> blend_mask);
