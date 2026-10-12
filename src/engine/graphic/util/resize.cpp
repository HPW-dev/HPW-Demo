#include "pch.hpp"
#include "resize.hpp"
#include "engine/graphic/image/image.hpp"
#include "engine/graphic/sprite/sprite.hpp"

void zoom_x2(Image& dst) {
  assert(dst);
  Image src(dst);
  dst.init(dst.X*2, dst.Y*2);

  cfor(y, src.Y)
  cfor(x, src.X) {
    cauto pix = src[x, y];
    dst[x*2+0, y*2+0] = pix;
    dst[x*2+1, y*2+0] = pix;
    dst[x*2+0, y*2+1] = pix;
    dst[x*2+1, y*2+1] = pix;
  }
}

void zoom_x2(Sprite& dst) {
  assert(dst);
  zoom_x2(dst.image());
  zoom_x2(dst.mask());
}

void zoom_x8(Image& dst) {
  assert(dst);
  Image src(dst);
  dst.init(dst.X*8, dst.Y*8);

  cfor(y, src.Y)
  cfor(x, src.X) {
    cauto pix = src[x, y];
    #define SET(_x, _y) dst[x * 8 + _x, y * 8 + _y] = pix;
    SET(0, 0) SET(1, 0) SET(2, 0) SET(3, 0) SET(4, 0) SET(5, 0) SET(6, 0) SET(7, 0)
    SET(0, 1) SET(1, 1) SET(2, 1) SET(3, 1) SET(4, 1) SET(5, 1) SET(6, 1) SET(7, 1)
    SET(0, 2) SET(1, 2) SET(2, 2) SET(3, 2) SET(4, 2) SET(5, 2) SET(6, 2) SET(7, 2)
    SET(0, 3) SET(1, 3) SET(2, 3) SET(3, 3) SET(4, 3) SET(5, 3) SET(6, 3) SET(7, 3)
    SET(0, 4) SET(1, 4) SET(2, 4) SET(3, 4) SET(4, 4) SET(5, 4) SET(6, 4) SET(7, 4)
    SET(0, 5) SET(1, 5) SET(2, 5) SET(3, 5) SET(4, 5) SET(5, 5) SET(6, 5) SET(7, 5)
    SET(0, 6) SET(1, 6) SET(2, 6) SET(3, 6) SET(4, 6) SET(5, 6) SET(6, 6) SET(7, 6)
    SET(0, 7) SET(1, 7) SET(2, 7) SET(3, 7) SET(4, 7) SET(5, 7) SET(6, 7) SET(7, 7)
    #undef SET
  }
} // zoom_x8

void zoom_x3(Image& dst) {
  assert(dst);
  Image src(dst);
  dst.init(dst.X*3, dst.Y*3);

  cfor(y, src.Y)
  cfor(x, src.X) {
    cauto pix = src[x, y];
    #define SET(_x, _y) dst[x * 3 + _x, y * 3 + _y] = pix;
    SET(0, 0) SET(1, 0) SET(2, 0)
    SET(0, 1) SET(1, 1) SET(2, 1)
    SET(0, 2) SET(1, 2) SET(2, 2)
    #undef SET
  }
} // zoom_x3

void zoom_x4(Image& dst) {
  assert(dst);
  Image src(dst);
  dst.init(dst.X*4, dst.Y*4);

  cfor(y, src.Y)
  cfor(x, src.X) {
    cauto pix = src[x, y];
    #define SET(_x, _y) dst[x * 4 + _x, y * 4 + _y] = pix;
    SET(0, 0) SET(1, 0) SET(2, 0) SET(3, 0)
    SET(0, 1) SET(1, 1) SET(2, 1) SET(3, 1)
    SET(0, 2) SET(1, 2) SET(2, 2) SET(3, 2)
    SET(0, 3) SET(1, 3) SET(2, 3) SET(3, 3)
    #undef SET
  }
} // zoom_x4

void zoom_x4(Sprite& dst) {
  assert(dst);
  zoom_x4(dst.image());
  zoom_x4(dst.mask());
}

void zoom_x8(Sprite& dst) {
  assert(dst);
  zoom_x8(dst.image());
  zoom_x8(dst.mask());
}

Image pixel_downscale_x3(cr<Image> src, Color_get_pattern cgp, Color_compute ccf) {
  return_if (!src, src);

  sconst std::unordered_map<Color_get_pattern, decltype(color_get_cross)*> cgp_table {
    {Color_get_pattern::cross, &color_get_cross},
    {Color_get_pattern::box, &color_get_box},
  };
  sconst std::unordered_map<Color_compute, decltype(most_common_col)*> ccf_table {
    {Color_compute::most_common, &most_common_col},
    {Color_compute::max, &max_col},
    {Color_compute::min, &min_col},
    {Color_compute::average, &average_col},
  };

  Image dst(src.X / 3, src.Y / 3);
  cauto selected_cgp = cgp_table.at(cgp);
  cauto selected_ccf = ccf_table.at(ccf);
  assert(dst.X > 2);
  assert(dst.Y > 2);

  #pragma omp parallel for simd collapse(2) schedule(static, 32)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    cauto colors = selected_cgp(src, x * 3, y * 3);
    cauto pix = selected_ccf(colors);
    dst.fast_set(x, y, pix, {});
  }
  return dst;
} // pixel_downscale_x3

Sprite pixel_upscale_x3(cr<Sprite> src) {
  if (!src) {
    log_warning << "WARNING: pixel_upscale_x3 empty src";
    return src;
  }
  Sprite dst;
  dst.move_image(std::move( pixel_upscale_x3(src.image()) ));
  dst.move_mask(std::move( pixel_upscale_x3(src.mask()) ));
  return dst;
}

Sprite pixel_downscale_x3(cr<Sprite> src, Color_get_pattern cgp, Color_compute ccf) {
  if ( !src) {
    log_warning << "WARNING: pixel_downscale_x3 empty src";
    return src;
  }
  Sprite dst;
  dst.move_image(std::move( pixel_downscale_x3(src.image(), cgp, ccf) ));
  dst.move_mask(std::move( pixel_downscale_x3(src.mask(), cgp, ccf) ));
  return dst;
}

Image pixel_upscale_x3(cr<Image> src) {
  Image dst(src.X * 3, src.Y * 3);

  #pragma omp parallel for simd schedule(static, 4) collapse(2)
  cfor (y, src.Y)
  cfor (x, src.X) {
    cauto A {src.get(x - 1, y - 1, Image_get::COPY)};
    cauto B {src.get(x + 0, y - 1, Image_get::COPY)};
    cauto C {src.get(x + 1, y - 1, Image_get::COPY)};
    cauto D {src.get(x - 1, y + 0, Image_get::COPY)};
    cauto E {src    [x,     y                     ]};
    cauto F {src.get(x + 1, y + 0, Image_get::COPY)};
    cauto G {src.get(x - 1, y + 1, Image_get::COPY)};
    cauto H {src.get(x + 0, y + 1, Image_get::COPY)};
    cauto I {src.get(x + 1, y + 1, Image_get::COPY)};

    Pal8 P1, P2, P3;
    Pal8 P4, P5, P6;
    Pal8 P7, P8, P9;

    if (B != H && D != F) {
      P1 = D == B ? D : E;
      P2 = (D == B && E != C) || (B == F && E != A) ? B : E;
      P3 = B == F ? F : E;
      P4 = (D == B && E != G) || (D == H && E != A) ? D : E;
      P5 = E;
      P6 = (B == F && E != I) || (H == F && E != C) ? F : E;
      P7 = D == H ? D : E;
      P8 = (D == H && E != I) || (H == F && E != G) ? H : E;
      P9 = H == F ? F : E;
    } else {
      P1 = E; P2 = E; P3 = E;
      P4 = E; P5 = E; P6 = E;
      P7 = E; P8 = E; P9 = E;
    }
    dst.fast_set(x * 3 + 0, y * 3 + 0, P1, {});
    dst.fast_set(x * 3 + 1, y * 3 + 0, P2, {});
    dst.fast_set(x * 3 + 2, y * 3 + 0, P3, {});
    dst.fast_set(x * 3 + 0, y * 3 + 1, P4, {});
    dst.fast_set(x * 3 + 1, y * 3 + 1, P5, {});
    dst.fast_set(x * 3 + 2, y * 3 + 1, P6, {});
    dst.fast_set(x * 3 + 0, y * 3 + 2, P7, {});
    dst.fast_set(x * 3 + 1, y * 3 + 2, P8, {});
    dst.fast_set(x * 3 + 2, y * 3 + 2, P9, {});
  }
  return dst;
} // pixel_upscale_x3

Pal8 most_common_col(const Pack9 colors) {
  std::size_t max_count {};
  Pal8 ret {};
  cauto colors_sz = colors.size;

  cfor (ai, colors_sz) {
    std::size_t count {};
    cfor (bi, colors_sz)
      if (colors.data[ai] == colors.data[bi])
        ++count;

    if (count > max_count) {
      max_count = count;
      ret = colors.data[ai];
    }
  } // for ai -> colors_sz

  return ret;
} // most_common_col

Pal8 max_col(const Pack9 colors)
  { return *std::max_element(colors.data, colors.data + colors.size); }

Pal8 min_col(const Pack9 colors)
  { return *std::min_element(colors.data, colors.data + colors.size); }

Pal8 average_col(const Pack9 colors) {
  real ret = 0;
  bool is_red = false;
  cauto colors_sz = colors.size;

  cfor (i, colors_sz) {
    cauto color = colors.data[i];
    is_red |= color.is_red();
    ret += color.to_real();
  }
  return Pal8::from_real(ret / colors_sz, is_red);
}

Pack9 color_get_cross(cr<Image> src, int x, int y) {
  Pack9 ret;
  ret.size = 5;
  constexpr static const auto mode = Image_get::MIRROR;
  ret.data[0] = src.get(x + 0, y - 1, mode);
  ret.data[1] = src.get(x - 1, y + 0, mode);
  ret.data[2] = src    [x + 0, y + 0      ];
  ret.data[3] = src.get(x + 1, y + 0, mode);
  ret.data[4] = src.get(x + 0, y + 1, mode);
  return ret;
}

Pack9 color_get_box(cr<Image> src, int x, int y) {
  Pack9 ret;
  ret.size = 9;
  constexpr static const auto mode = Image_get::MIRROR;
  ret.data[0] = src.get(x - 1, y - 1, mode);
  ret.data[1] = src.get(x + 0, y - 1, mode);
  ret.data[2] = src.get(x + 1, y - 1, mode);
  ret.data[3] = src.get(x - 1, y + 0, mode);
  ret.data[4] = src    [x + 0, y + 0      ];
  ret.data[5] = src.get(x + 1, y + 0, mode);
  ret.data[6] = src.get(x - 1, y + 1, mode);
  ret.data[7] = src.get(x + 0, y + 1, mode);
  return ret;
}

Str convert(Color_compute ccf) {
  std::unordered_map<Color_compute, Str> table {
    {Color_compute::most_common, "most common"},
    {Color_compute::max, "max"},
    {Color_compute::min, "min"},
    {Color_compute::average, "average"},
  };
  return table.at(ccf);
}

Str convert(Color_get_pattern cgp) {
  std::unordered_map<Color_get_pattern, Str> table {
    {Color_get_pattern::cross, "cross"},
    {Color_get_pattern::box, "box"},
  };
  return table.at(cgp);
}

Color_compute convert_to_ccf(cr<Str> name) {
  std::unordered_map<Str, Color_compute> table {
    {"most common", Color_compute::most_common},
    {"max", Color_compute::max},
    {"min", Color_compute::min},
    {"average", Color_compute::average},
  };
  return table.at(name);
}

Color_get_pattern convert_to_cgp(cr<Str> name) {
  std::unordered_map<Str, Color_get_pattern> table {
    {"cross", Color_get_pattern::cross},
    {"box", Color_get_pattern::box},
  };
  return table.at(name);
}

static inline real blerp(real c00, real c10, real c01,
real c11, real tx, real ty) {
  return std::lerp(std::lerp(c00, c10, tx), std::lerp(c01, c11, tx), ty);
}

Image resize_bilinear(cr<Image> src, const uint NEW_SIZE_X, const uint NEW_SIZE_Y) {
  assert(src);
  return_if(NEW_SIZE_X == scast<uint>(src.X) && NEW_SIZE_Y == scast<uint>(src.Y), src);
  return_if(NEW_SIZE_X == 0 || NEW_SIZE_Y == 0, {});

  Image ret(NEW_SIZE_X, NEW_SIZE_Y);
  const real scale_x = 1.0 / (scast<real>(NEW_SIZE_X) / src.X);
  const real scale_y = 1.0 / (scast<real>(NEW_SIZE_Y) / src.Y);

  cfor (y, ret.Y)
  cfor (x, ret.X) {
    const real dx = x * scale_x;
    const real dy = y * scale_y;
    const int gxi = std::floor(dx);
    const int gyi = std::floor(dy);
    cauto c00 =     src[gxi,     gyi];
    cauto c10 = src.get(gxi + 1, gyi);
    cauto c01 = src.get(gxi,     gyi + 1);
    cauto c11 = src.get(gxi + 1, gyi + 1);
    cauto tx = dx - gxi;
    cauto ty = dy - gyi;
    // blerp
    cauto a = blend_alpha(c00, c10, tx * 255);
    cauto b = blend_alpha(c01, c11, tx * 255);
    cauto c = blend_alpha(a, b, ty * 255);
    ret.fast_set(x, y, c, {});
  }

  return ret;
}

Image resize_area(cr<Image> src, const uint NEW_SIZE_X, const uint NEW_SIZE_Y) {
  // (c) Gemini
  assert(src);
  return_if(NEW_SIZE_X == scast<uint>(src.X) && NEW_SIZE_Y == scast<uint>(src.Y), src);
  return_if(NEW_SIZE_X == 0 || NEW_SIZE_Y == 0, {});

  // при увеличении, area не подходит
  if (NEW_SIZE_X > scast<uint>(src.X) || NEW_SIZE_Y > scast<uint>(src.Y))
    return resize_bilinear(src, NEW_SIZE_X, NEW_SIZE_Y);

  Image ret(NEW_SIZE_X, NEW_SIZE_Y);

  // Коэффициенты масштабирования (сколько старых пикселей помещается в один новый)
  const real scale_x = scast<real>(src.X) / NEW_SIZE_X;
  const real scale_y = scast<real>(src.Y) / NEW_SIZE_Y;

  cfor (y, ret.Y)
  cfor (x, ret.X) {
    // Границы целевого пикселя (x, y) в координатах исходного изображения
    const real src_x_start = x * scale_x;
    const real src_x_end   = (x + 1) * scale_x;
    const real src_y_start = y * scale_y;
    const real src_y_end   = (y + 1) * scale_y;

    // Индексы исходных пикселей, которые мы задеваем
    const int x_first = std::floor(src_x_start);
    const int x_last  = std::min(scast<int>(std::ceil(src_x_end)), src.X) - 1;
    const int y_first = std::floor(src_y_start);
    const int y_last  = std::min(scast<int>(std::ceil(src_y_end)), src.Y) - 1;

    // Аккумуляторы для каналов цвета (предполагаем floats/doubles для точности)
    real sum = 0;
    real total_weight = 0;

    // Проходим по всем исходным пикселям, попавшим в зону покрытия
    for (int sy = y_first; sy <= y_last; ++sy) {
      // Вычисляем высоту пересечения пикселя sy с целевой зоной
      const real y_weight = std::min(scast<real>(sy + 1),
        src_y_end) - std::max(scast<real>(sy), src_y_start);
      cont_if (y_weight <= 0);

      for (int sx = x_first; sx <= x_last; ++sx) {
        // Вычисляем ширину пересечения пикселя sx с целевой зоной
        const real x_weight = std::min(scast<real>(sx + 1),
          src_x_end) - std::max(scast<real>(sx), src_x_start);
        cont_if (x_weight <= 0);

        const real weight = x_weight * y_weight;
        cauto color = src[sx, sy]; // Предполагается, что у вас есть оператор [x, y]

        // Умножаем каналы цвета на их вес (площадь покрытия)
        sum += color.to_real() * weight;
        total_weight += weight;
      }
    }

    // Финальный цвет — это взвешенное среднее
    Pal8 final_color {};
    if (total_weight > 0)
      final_color = Pal8::from_real(sum / total_weight);

    ret.fast_set(x, y, final_color, {});
  }

  return ret;
}

Image resize_neighbor(cr<Image> src, const uint NEW_SIZE_X, const uint NEW_SIZE_Y) {
  assert(src);
  return_if(NEW_SIZE_X == scast<uint>(src.X) && NEW_SIZE_Y == scast<uint>(src.Y), src);
  return_if(NEW_SIZE_X == 0 || NEW_SIZE_Y == 0, {});

  Image ret(NEW_SIZE_X, NEW_SIZE_Y);
  const real scale_x = 1.0 / (scast<real>(NEW_SIZE_X) / src.X);
  const real scale_y = 1.0 / (scast<real>(NEW_SIZE_Y) / src.Y);

  #pragma omp parallel for simd collapse(2) if (ret.X * ret.Y >= 64 * 64)
  cfor (y, ret.Y)
  cfor (x, ret.X) {
    const int gxi = std::floor(x * scale_x);
    const int gyi = std::floor(y * scale_y);
    cauto color = src[gxi, gyi];
    ret[x, y] = color;
  }

  return ret;
}

void downscale_x2(Image& dst, cr<Image> src) {
  assert(src);
  dst.init(src.X/2, src.Y/2);
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    cauto p00 = src[x*2 + 0, y*2 + 0];
    cauto p10 = src[x*2 + 1, y*2 + 0];
    cauto a = blend_avr(p00, p10);
    cauto p01 = src[x*2 + 0, y*2 + 1];
    cauto p11 = src[x*2 + 1, y*2 + 1];
    cauto b = blend_avr(p01, p11);
    cauto c = blend_avr(a, b);
    dst[x, y] = c;
  }
}

void downscale_x3(Image& dst, cr<Image> src) {
  assert(src);
  dst.init(src.X/3, src.Y/3);
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    cauto p00 = src[x*3 + 0, y*3 + 0];
    cauto p01 = src[x*3 + 0, y*3 + 1];
    cauto p02 = src[x*3 + 0, y*3 + 2];
    cauto a = blend_avr(p00, blend_avr(p01, p02));

    cauto p10 = src[x*3 + 1, y*3 + 0];
    cauto p11 = src[x*3 + 1, y*3 + 1];
    cauto p12 = src[x*3 + 1, y*3 + 2];
    cauto b = blend_avr(p10, blend_avr(p11, p12));

    cauto p20 = src[x*3 + 2, y*3 + 0];
    cauto p21 = src[x*3 + 2, y*3 + 1];
    cauto p22 = src[x*3 + 2, y*3 + 2];
    cauto c = blend_avr(p20, blend_avr(p21, p22));

    cauto d = blend_avr(a, blend_avr(b, c));
    dst[x, y] = d;
  }
}

void downscale_x4(Image& dst, cr<Image> src) {
  downscale_x2(dst, src);
  downscale_x2(dst, Image(dst));
}
