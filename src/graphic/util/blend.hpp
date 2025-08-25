#pragma once
#include "util/macro.hpp"
#include "graphic/image/image-fwd.hpp"

/** накладывает src на dst с картой прозрачности в blend_mask
* @details dst, src и blend_mask должны быть одинаковы по размеру */
void accept_blend_mask_fast(Image& dst, cr<Image> src, cr<Image> blend_mask);
