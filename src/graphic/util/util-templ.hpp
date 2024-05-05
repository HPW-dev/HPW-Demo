#pragma once
// @file template funcs
#include <cassert>
#include <utility>
#include <algorithm>
#include <cmath>
#include "graphic/sprite/sprite.hpp"
#include "graphic/image/image.hpp"
#include "graphic/image/color-blend.hpp"
//#include "graphic/util/graphic-util.hpp"
#include "util/math/vec.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/rect.hpp"
#include "util/error.hpp"

extern Rect get_insertion_bound(CN<Image> dst, const Vec pos, CN<Image> src);

// нарисовать прямоуг-к
template <blend_pf bf = &blend_past>
void draw_rect(Image& dst, CN<Rect> rect, const Pal8 col);

// нарисовать залитый прямоуг-к
template <blend_pf bf = &blend_past>
void draw_rect_filled(Image& dst, CN<Rect> rect, const Pal8 col);

// нарисовать залитый круг
template <blend_pf bf = &blend_past>
void draw_circle_filled(Image& dst, const Vec pos,
  int radius, const Pal8 col);

// копирует одну картинку в другую
template <blend_pf bf = &blend_past>
void insert(Image& dst, CN<Image> src, Vec pos={}, int optional=0);

// вставка с шахматным миганием
template<blend_pf bf = &blend_past>
void insert_blink(Image& dst, CN<Image> src, Vec pos={},
  int timer=0, int optional=0);

// вставка с черезстрочным миганием
template<blend_pf bf = &blend_past>
void insert_interlace(Image& dst, CN<Image> src, Vec pos={},
  int timer=0, int optional=0);

// отрисовка линии
template<blend_pf bf = &blend_past>
void draw_line(Image& dst, Vec p1, const Vec p2, Pal8 color);

// копирует спрайт в картинку (шаблонная версия)
template <blend_pf bf = &blend_past>
void insert(Image& dst, CN<Sprite> src, Vec pos={}, int optional=0);

// круг без заливки
template <blend_pf bf = &blend_past>
void draw_circle(Image& dst, const Vec pos, int radius,
  const Pal8 col);

// рисует крестик
template <blend_pf bf = &blend_past>
void draw_cross(Image& dst, const Vec pos, const Pal8 col,
  uint size=3);

// рисует диагональный крестик
template <blend_pf bf = &blend_past>
void draw_cross_diagonal(Image& dst, const Vec pos, const Pal8 col,
  uint size=3);

template <blend_pf bf>
void insert_fast(Image& dst, CN<Image> src, int optional={});

// ----------------------- impl ----------------------------------

template <blend_pf bf>
void draw_rect(Image& dst, CN<Rect> rect, const Pal8 col) {
  return_if( !dst);
  return_if(rect.size.x <= 2 || rect.size.y <= 2);
  const int rect_pos_x = std::round(rect.pos.x);
  const int rect_pos_y = std::round(rect.pos.y);
  const int rect_sz_x = std::round(rect.size.x);
  const int rect_sz_y = std::round(rect.size.y);
  cauto ex = rect_pos_x + rect_sz_x;
  cauto ey = rect_pos_y + rect_sz_y;
  // рисование линий по два раза:
  for (int x = rect_pos_x; x < ex; ++x) {
    dst.set<bf>(x, rect_pos_y, col);
    dst.set<bf>(x, ey-1, col);
  }
  for (int y = rect_pos_y+1; y < ey-1; ++y) {
    dst.set<bf>(rect_pos_x, y, col);
    dst.set<bf>(ex-1, y, col);
  }
} // draw_rect

template <blend_pf bf>
void draw_rect_filled(Image& dst, CN<Rect> rect, const Pal8 col) {
  return_if( !dst);
  return_if (rect.size.x < 1 || rect.size.y < 1);
  cauto ex = rect.pos.x + rect.size.x;
  cauto ey = rect.pos.y + rect.size.y;
  for (int y = rect.pos.y; y < ey; ++y)
  for (int x = rect.pos.x; x < ex; ++x)
    dst.set<bf>(x, y, col);
} // draw_rect_filled

template <blend_pf bf>
void draw_circle_filled(Image& dst, const Vec pos,
int radius, const Pal8 col) {
  return_if( !dst);
  int x = radius;
  int y = 0;
  int xChange = 1 - (radius << 1);
  int yChange = 0;
  int radiusError = 0;
  while (x >= y) {
    for (int i = pos.x - x; i <= pos.x + x; ++i) {
      dst.set<bf>(i, pos.y + y, col);
      dst.set<bf>(i, pos.y - y, col);
    }
    for (int i = pos.x - y; i <= pos.x + y; ++i) {
      dst.set<bf>(i, pos.y + x, col);
      dst.set<bf>(i, pos.y - x, col);
    }
    ++y;
    radiusError += yChange;
    yChange += 2;
    if (((radiusError << 1) + xChange) > 0) {
      --x;
      radiusError += xChange;
      xChange += 2;
    }
  } // while (x >= y)
} // draw_circle_filled

template <blend_pf bf>
void insert(Image& dst, CN<Image> src, Vec pos, int optional) {
  return_if( !src || !dst);
  
  pos = floor(pos);
  auto bound = get_insertion_bound(dst, pos, src);
  return_if (bound.size.x == 0 || bound.size.y == 0);

  auto sy = scast<int>(bound.pos.y);
  auto sx = scast<int>(bound.pos.x);
  auto ey = scast<int>(bound.pos.y + bound.size.y);
  auto ex = scast<int>(bound.pos.x + bound.size.x);
  auto dst_p = dst.data();
  auto src_p = src.data();
  int dst_front_poch = std::max<real>(0, pos.x);
  int dst_back_porch = (dst.X - (ex - sx)) - std::max<real>(0, pos.x);
  int src_front_poch = bound.pos.x;
  int src_back_porch = src.X - (bound.pos.x + bound.size.x);

  // начальное смещение src по y
  src_p += sy * src.X;
  // начальное смещение dst по y с оффсетом
  dst_p += (sy + scast<int>(pos.y)) * dst.X;

  for (auto y = sy; y < ey; ++y) {
    // смещение по x (левый пич)
    dst_p += dst_front_poch;
    src_p += src_front_poch;

    // отрисовка строки
    for (auto x = sx; x < ex; ++x) {
      *dst_p = bf(*src_p, *dst_p, optional);

      // движение по строке
      ++src_p;
      ++dst_p;
    }

    // смещение по x (правый пич)
    dst_p += dst_back_porch;
    src_p += src_back_porch;
  }
} // insert

template<blend_pf bf>
void insert_blink(Image& dst, CN<Image> src, Vec pos,
int timer, int optional) {
  return_if( !src || !dst);

  constexpr int dots2x2[2][2] = {
    {0, 1},
    {1, 0} };
  
  pos = floor(pos);
  auto bound = get_insertion_bound(dst, pos, src);
  return_if (bound.size.x == 0 || bound.size.y == 0);

  auto sy = scast<int>(bound.pos.y);
  auto sx = scast<int>(bound.pos.x);
  auto ey = scast<int>(bound.pos.y + bound.size.y);
  auto ex = scast<int>(bound.pos.x + bound.size.x);
  auto dst_p = dst.data();
  auto src_p = src.data();
  int dst_front_poch = std::max<real>(0, pos.x);
  int dst_back_porch = (dst.X - (ex - sx)) - std::max<real>(0, pos.x);
  int src_front_poch = bound.pos.x;
  int src_back_porch = src.X - (bound.pos.x + bound.size.x);

  // начальное смещение src по y
  src_p += sy * src.X;
  // начальное смещение dst по y с оффсетом
  dst_p += (sy + scast<int>(pos.y)) * dst.X;

  for (auto y = sy; y < ey; ++y) {
    // смещение по x (левый пич)
    dst_p += dst_front_poch;
    src_p += src_front_poch;

    // отрисовка строки
    for (auto x = sx; x < ex; ++x) {
      Pal8 SRC_DST[2] = {*src_p, *dst_p};
      const int mode = dots2x2[x & 1][y & 1] ^ (timer & 1);
      SRC_DST[0] = SRC_DST[mode];
      *dst_p = bf(SRC_DST[mode], *dst_p, optional);

      // движение по строке
      ++src_p;
      ++dst_p;
    }

    // смещение по x (правый пич)
    dst_p += dst_back_porch;
    src_p += src_back_porch;
  }
} // insert_blink

template<blend_pf bf>
void insert_interlace(Image& dst, CN<Image> src, Vec pos,
int timer, int optional) {
  return_if( !src || !dst);
  
  pos = floor(pos);
  auto bound = get_insertion_bound(dst, pos, src);
  return_if (bound.size.x == 0 || bound.size.y == 0);

  auto sy = scast<int>(bound.pos.y);
  auto sx = scast<int>(bound.pos.x);
  auto ey = scast<int>(bound.pos.y + bound.size.y);
  auto ex = scast<int>(bound.pos.x + bound.size.x);
  auto dst_p = dst.data();
  auto src_p = src.data();
  int dst_front_poch = std::max<real>(0, pos.x);
  int dst_back_porch = (dst.X - (ex - sx)) - std::max<real>(0, pos.x);
  int src_front_poch = bound.pos.x;
  int src_back_porch = src.X - (bound.pos.x + bound.size.x);

  // начальное смещение src по y
  src_p += sy * src.X;
  // начальное смещение dst по y с оффсетом
  dst_p += (sy + scast<int>(pos.y)) * dst.X;

  for (auto y = sy; y < ey; ++y) {
    if ((y & 1) ^ (timer & 1)) {
      dst_p += dst.X;
      src_p += src.X;
      continue;
    }

    // смещение по x (левый пич)
    dst_p += dst_front_poch;
    src_p += src_front_poch;

    // отрисовка строки
    for (auto x = sx; x < ex; ++x) {
      *dst_p = bf(*src_p, *dst_p, optional);

      // движение по строке
      ++src_p;
      ++dst_p;
    }

    // смещение по x (правый пич)
    dst_p += dst_back_porch;
    src_p += src_back_porch;
  }
} // insert_blink

template<blend_pf bf>
void draw_line(Image& dst, Vec _p1, const Vec _p2, Pal8 color) {
  struct Veci { int x {}, y {}; };
  auto floored_p1 = floor(_p1);
  Veci p1 {.x = scast<int>(floored_p1.x), .y = scast<int>(floored_p1.y)};
  auto floored_p2 = floor(_p2);
  Veci p2 {.x = scast<int>(floored_p2.x), .y = scast<int>(floored_p2.y)};
  
  // EFLA Variation E (Addition Fixed Point PreCalc)
  return_if( !dst);
  // выход, если линия за пределами видимости
  if (p1.x < 0 && p2.x < 0)
    return;
  if (p1.x >= dst.X && p2.x >= dst.X)
    return;
  if (p1.y < 0 && p2.y < 0)
    return;
  if (p1.y >= dst.Y && p2.y >= dst.Y)
    return; 
  bool yLonger = false;
  int shortLen = p2.y - p1.y;
  int longLen = p2.x - p1.x;
  if (std::abs(shortLen) > std::abs(longLen)) {
    int swap = shortLen;
    shortLen = longLen;
    longLen = swap;				
    yLonger = true;
  }
  int decInc;
  if (longLen == 0)
    decInc = 0;
  else
    decInc = (shortLen << 16) / longLen;
  if (yLonger) {
    if (longLen > 0) {
      longLen += p1.y;
      for (int j = 0x8000 + (p1.x << 16); p1.y <= longLen; ++p1.y) {
        dst.set<bf>(j >> 16, p1.y, color);
        j += decInc;
      }
      return;
    }
    longLen += p1.y;
    for (int j = 0x8000 + (p1.x << 16); p1.y >= longLen; --p1.y) {
      dst.set<bf>(j >> 16, p1.y, color);	
      j -= decInc;
    }
    return;	
  }
  if (longLen > 0) {
    longLen += p1.x;
    for (int j = 0x8000 + (p1.y << 16); p1.x <= longLen; ++p1.x) {
      dst.set<bf>(p1.x, j >> 16, color);
      j += decInc;
    }
    return;
  }
  longLen += p1.x;
  for (int j = 0x8000 + (p1.y << 16); p1.x >= longLen; --p1.x) {
    dst.set<bf>(p1.x, j >> 16, color);
    j -= decInc;
  }
} // draw_line

template <blend_pf bf>
void insert(Image& dst, CN<Sprite> src, Vec pos, int optional) {
  return_if( !src || !dst);
  nauto src_image {*src.get_image()};
  nauto src_mask {*src.get_mask()};
  
  pos = floor(pos);
  auto bound = get_insertion_bound(dst, pos, src_image);
  return_if (bound.size.x == 0 || bound.size.y == 0);

  auto sy = scast<int>(bound.pos.y);
  auto sx = scast<int>(bound.pos.x);
  auto ey = scast<int>(bound.pos.y + bound.size.y);
  auto ex = scast<int>(bound.pos.x + bound.size.x);
  auto dst_p = dst.data();
  auto src_p = src_image.data();
  auto mask_p = src_mask.data();
  int dst_front_poch = std::max<real>(0, pos.x);
  int dst_back_porch = (dst.X - (ex - sx)) - std::max<real>(0, pos.x);
  int src_front_poch = bound.pos.x;
  int src_back_porch = src_image.X - (bound.pos.x + bound.size.x);

  // начальное смещение src по y
  src_p += sy * src_image.X;
  mask_p += sy * src_image.X;
  // начальное смещение dst по y с оффсетом
  dst_p += (sy + scast<int>(pos.y)) * dst.X;

  for (auto y = sy; y < ey; ++y) {
    // смещение по x (левый пич)
    dst_p += dst_front_poch;
    mask_p += src_front_poch;
    src_p += src_front_poch;

    // отрисовка строки
    for (auto x = sx; x < ex; ++x) {
      // табличная оптимизация без использования ветвления:
      const Pal8 SRC_DST[2] {bf(*src_p, *dst_p, optional), *dst_p};
      *dst_p = SRC_DST[mask_p->val & 1];

      // движение по строке
      ++src_p;
      ++mask_p;
      ++dst_p;
    }

    // смещение по x (правый пич)
    dst_p += dst_back_porch;
    src_p += src_back_porch;
    mask_p += src_back_porch;
  }
} // insert (sprite)

template <blend_pf bf>
void draw_circle(Image& dst, const Vec _pos, int radius, const Pal8 col) {
  if ( !dst || radius < 1) return;
  auto pos = floor(_pos);
  int x = radius;
  int y = 0;
  int err = 0;
  while (x >= y) {
    dst.set<bf>(pos.x + x, pos.y + y, col);
    dst.set<bf>(pos.x + y, pos.y + x, col);
    dst.set<bf>(pos.x - y, pos.y + x, col);
    dst.set<bf>(pos.x - x, pos.y + y, col);
    dst.set<bf>(pos.x - x, pos.y - y, col);
    dst.set<bf>(pos.x - y, pos.y - x, col);
    dst.set<bf>(pos.x + y, pos.y - x, col);
    dst.set<bf>(pos.x + x, pos.y - y, col);
    if (err <= 0) {
      ++y;
      err += 2 * y + 1;
    }
    if (err > 0) {
      --x;
      err -= 2 * x + 1;
    }
  } // while x >= y
} // draw_circle

template <blend_pf bf>
void draw_cross(Image& dst, const Vec pos, const Pal8 col,
uint size) {
  return_if(size == 0);
  assert(dst);
  for (int x = pos.x - scast<int>(size); x < pos.x; ++x)
    dst.set<bf>(x, pos.y, col);
  for (int x = pos.x + 1; x < pos.x + scast<int>(size) + 1; ++x)
    dst.set<bf>(x, pos.y, col);
  for (int y = pos.y - scast<int>(size); y < pos.y; ++y)
    dst.set<bf>(pos.x, y, col);
  for (int y = pos.y + 1; y < pos.y + scast<int>(size) + 1; ++y)
    dst.set<bf>(pos.x, y, col);
  dst.set<bf>(pos.x, pos.y, col);
} // draw_cross

template <blend_pf bf>
void draw_cross_diagonal(Image& dst, const Vec pos, const Pal8 col,
uint size) {
  return_if(size == 0);
  assert(dst);
  error("draw_cross_diagonal: need impl");
}

template <blend_pf bf>
void insert_fast(Image& dst, CN<Image> src, int optional) {
  cfor (i, dst.size)
    dst[i] = bf(src[i], dst[i], optional);
}
