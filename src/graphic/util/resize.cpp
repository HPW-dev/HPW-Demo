#include <omp.h>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <cassert>
#include "resize.hpp"
#include "graphic/image/image.hpp"
#include "graphic/sprite/sprite.hpp"
#include "util/log.hpp"

void zoom_x2(Image& dst) {
  assert(dst);
  Image src(dst);
  dst.init(dst.X*2, dst.Y*2);

  cfor(y, src.Y)
  cfor(x, src.X) {
    cauto pix = src(x, y);
    dst(x*2+0, y*2+0) = pix;
    dst(x*2+1, y*2+0) = pix;
    dst(x*2+0, y*2+1) = pix;
    dst(x*2+1, y*2+1) = pix;
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
    cauto pix = src(x, y);
    #define SET(_x, _y) dst(x * 8 + _x, y * 8 + _y) = pix;
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
    cauto pix = src(x, y);
    #define SET(_x, _y) dst(x * 3 + _x, y * 3 + _y) = pix;
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
    cauto pix = src(x, y);
    #define SET(_x, _y) dst(x * 4 + _x, y * 4 + _y) = pix;
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

// ускоренная версия для box + must_common
inline Image pixel_downscale_x3_box_mc(CN<Image> src) {
  static Image dst; // prebuf opt
  dst.assign_resize(src.X / 3, src.Y / 3);
  constexpr const uint max_c = 9;
  Pal8 c[max_c];
  
  #pragma omp parallel for simd schedule(static, 4) collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    // box
    c[0] = src.get(x*3 - 1, y*3 - 1, Image_get::MIRROR);
    c[1] = src.get(x*3 + 0, y*3 - 1, Image_get::MIRROR);
    c[2] = src.get(x*3 + 1, y*3 - 1, Image_get::MIRROR);
    c[3] = src.get(x*3 - 1, y*3 + 0, Image_get::MIRROR);
    c[4] =     src(x*3 + 0, y*3 + 0);
    c[5] = src.get(x*3 + 1, y*3 + 0, Image_get::MIRROR);
    c[6] = src.get(x*3 - 1, y*3 + 1, Image_get::MIRROR);
    c[7] = src.get(x*3 + 0, y*3 + 1, Image_get::MIRROR);
    c[8] = src.get(x*3 + 1, y*3 + 1, Image_get::MIRROR);

    // must common
    std::size_t max_count {};
    Pal8 pix;
    cfor (ai, max_c) {
      std::size_t count {};
      cfor (bi, max_c)
        if (c[ai] == c[bi])
          ++count;
      if (count > max_count) {
        max_count = count;
        pix = c[ai];
      }
    } // for ai

    dst.fast_set(x, y, pix, {});
  }
  return dst;
} // pixel_downscale_x3_box_mc

Image pixel_downscale_x3(CN<Image> src, Color_get_pattern cgp, Color_compute ccf) {
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

  if (cgp == Color_get_pattern::box && ccf == Color_compute::most_common)
    return pixel_downscale_x3_box_mc(src);

  static Image dst; // prebuf opt
  dst.assign_resize(src.X / 3, src.Y / 3);
  
  #pragma omp parallel for simd schedule(static, 4) collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    cauto colors = cgp_table.at(cgp) (src, x * 3, y * 3);
    cauto pix = ccf_table.at(ccf) (colors);
    dst.fast_set(x, y, pix, {});
  }
  return dst;
} // pixel_downscale_x3

Sprite pixel_upscale_x3(CN<Sprite> src) {
  if (!src) {
    hpw_log("WARNING: pixel_upscale_x3 empty src\n")
    return src;
  }
  Sprite dst;
  dst.move_image(std::move( pixel_upscale_x3(src.image()) ));
  dst.move_mask(std::move( pixel_upscale_x3(src.mask()) ));
  return dst;
}

Sprite pixel_downscale_x3(CN<Sprite> src, Color_get_pattern cgp, Color_compute ccf) {
  if ( !src) {
    hpw_log("WARNING: pixel_downscale_x3 empty src\n")
    return src;
  }
  Sprite dst;
  dst.move_image(std::move( pixel_downscale_x3(src.image(), cgp, ccf) ));
  dst.move_mask(std::move( pixel_downscale_x3(src.mask(), cgp, ccf) ));
  return dst;
}

Image pixel_upscale_x3(CN<Image> src) {
  static Image dst; // prebuf opt
  dst.assign_resize(src.X * 3, src.Y * 3);
  Pal8 P1, P2, P3;
  Pal8 P4, P5, P6;
  Pal8 P7, P8, P9;

  #pragma omp parallel for simd schedule(static, 4) collapse(2)
  cfor (y, src.Y)
  cfor (x, src.X) {
    auto A {src.get(x - 1, y - 1, Image_get::COPY)};
    auto B {src.get(x + 0, y - 1, Image_get::COPY)};
    auto C {src.get(x + 1, y - 1, Image_get::COPY)};
    auto D {src.get(x - 1, y + 0, Image_get::COPY)};
    auto E {src(x, y)};
    auto F {src.get(x + 1, y + 0, Image_get::COPY)};
    auto G {src.get(x - 1, y + 1, Image_get::COPY)};
    auto H {src.get(x + 0, y + 1, Image_get::COPY)};
    auto I {src.get(x + 1, y + 1, Image_get::COPY)};
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

Pal8 most_common_col(CN<Vector<Pal8>> colors) {
  std::size_t max_count {};
  CP<Pal8> ret {};

  for (cnauto a: colors) {
    std::size_t count {};
    for (cnauto b: colors)
      if (a == b)
        ++count;

    if (count > max_count) {
      max_count = count;
      ret = &a;
    }
  } // for a: colors

  return *ret;
} // most_common_col

Pal8 max_col(CN<Vector<Pal8>> colors) {
  return *std::max_element(colors.begin(), colors.end());
}

Pal8 min_col(CN<Vector<Pal8>> colors) {
  return *std::min_element(colors.begin(), colors.end());
}

Pal8 average_col(CN<Vector<Pal8>> colors) {
  real ret = 0;
  bool is_red = false;
  for (cnauto color: colors) {
    is_red |= color.is_red();
    ret += color.to_real();
  }
  return Pal8::from_real(ret / colors.size(), is_red);
}

Vector<Pal8> color_get_cross(CN<Image> src, int x, int y) {
  Vector<Pal8> ret(5);
  auto mode = Image_get::MIRROR;
  ret[0] = src.get(x + 0, y - 1, mode);
  ret[1] = src.get(x - 1, y + 0, mode);
  ret[2] =     src(x + 0, y + 0);
  ret[3] = src.get(x + 1, y + 0, mode);
  ret[4] = src.get(x + 0, y + 1, mode);
  return ret;
}

Vector<Pal8> color_get_box(CN<Image> src, int x, int y) {
  Vector<Pal8> ret(9);
  auto mode = Image_get::MIRROR;
  ret[0] = src.get(x - 1, y - 1, mode);
  ret[1] = src.get(x + 0, y - 1, mode);
  ret[2] = src.get(x + 1, y - 1, mode);
  ret[3] = src.get(x - 1, y + 0, mode);
  ret[4] =     src(x + 0, y + 0);
  ret[5] = src.get(x + 1, y + 0, mode);
  ret[6] = src.get(x - 1, y + 1, mode);
  ret[7] = src.get(x + 0, y + 1, mode);
  ret[8] = src.get(x + 1, y + 1, mode);
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

Color_compute convert_to_ccf(Str name) {
  std::unordered_map<Str, Color_compute> table {
    {"most common", Color_compute::most_common},
    {"max", Color_compute::max},
    {"min", Color_compute::min},
    {"average", Color_compute::average},
  };
  return table.at(name);
}

Color_get_pattern convert_to_cgp(Str name) {
  std::unordered_map<Str, Color_get_pattern> table {
    {"cross", Color_get_pattern::cross},
    {"box", Color_get_pattern::box},
  };
  return table.at(name);
}
