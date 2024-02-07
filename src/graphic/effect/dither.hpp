#pragma once
#include "util/macro.hpp"
#include "graphic/image/color-blend.hpp"
#include "util/math/vec.hpp"

class Image;
class Sprite;

/// типы дизеров
enum class Dither {
  none,
  noise,
  hline, /// horizontal line
  error, /// error diffusion
  bayer2x2,
  bayer16x16,
};

/// смешивание картинки с дизерингом за место усреднения
void dither_blend(Image& dst, CN<Image> src, const Vec pos, real alpha,
  Dither type=Dither::bayer16x16, blend_pf bf=&blend_past);
/// смешивание спрайта и картинки с дизерингом за место усреднения
void dither_blend(Image& dst, CN<Sprite> src, const Vec pos, real alpha,
  Dither type=Dither::bayer16x16, blend_pf bf=&blend_past);
/// быстрый 16x16 4-бит дизер
void fast_dither_bayer16x16_4bit(Image& dst, bool rotate_pattern=false);