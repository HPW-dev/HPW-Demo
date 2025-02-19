#include <algorithm>
#include <cstring>
#include <utility>
#include <unordered_map>
#include <cmath>
#include "graphic-util.hpp"
#include "util-templ.hpp"
#include "graphic/image/image.hpp"
#include "graphic/sprite/sprite.hpp"
#include "util/math/random.hpp"
#include "util/math/polygon.hpp"
#include "util/math/mat.hpp"
#include "util/log.hpp"
#include "game/util/sync.hpp"
#include "game/core/graphic.hpp"

void insert_fast(Image& dst, cr<Image> src) {
  return_if(!dst);
  assert(dst.size >= src.size);
  memcpy(dst.data(), src.data(), dst.size * sizeof(Pal8));
}

void insert(Image& dst, cr<Image> src, Vec pos, blend_pf bf,
int optional) {
  assert(src);
  assert(dst);
  
  pos = floor(pos);
  auto bound = get_insertion_bound(dst, pos, src);
  return_if (bound.size.x == 0 || bound.size.y == 0);

  auto sy = scast<int>(bound.pos.y);
  auto sx = scast<int>(bound.pos.x);
  auto ey = scast<int>(bound.pos.y + bound.size.y);
  auto ex = scast<int>(bound.pos.x + bound.size.x);
  auto dst_p = dst.data();
  auto src_p = src.data();
  int dst_front_porch = std::max<real>(0, pos.x);
  int dst_back_porch = (dst.X - (ex - sx)) - std::max<real>(0, pos.x);
  int src_front_porch = bound.pos.x;
  int src_back_porch = src.X - (bound.pos.x + bound.size.x);

  // начальное смещение src по y
  src_p += sy * src.X;
  // начальное смещение dst по y с оффсетом
  dst_p += (sy + scast<int>(pos.y)) * dst.X;

  for (auto y = sy; y < ey; ++y) {
    // смещение по x (левый пич)
    dst_p += dst_front_porch;
    src_p += src_front_porch;

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

Pal8 rand_color_stable(bool red) {
  Pal8::value_t val;
  if (red)
    val = rnd(Pal8::red_start, Pal8::red);
  else
    val = rndu(Pal8::gray_end);
  return {val};
}

Pal8 rand_color_graphic(bool red) {
  Pal8::value_t val;
  if (red)
    val = rnd_fast(Pal8::red_start, Pal8::red);
  else
    val = rndu_fast(Pal8::gray_end);
  return {val};
}

Vec center_point(cr<Image> src)
  { return Vec(src.X / 2.0, src.Y / 2.0); }

Vec center_point(cr<Sprite> src) {
  if (!src) {
    log_warning << "WARNING: center_point src is empty";
    return {};
  }
  return Vec{src.X() / 2.0, src.Y() / 2.0};
}

Vec center_point(cr<Image> src, cr<Image> dst)
  { return Vec(src.X / 2.0 - dst.X / 2.0, src.Y / 2.0 - dst.Y / 2.0); }

Vec center_point(cr<Image> src, cr<Sprite> dst) {
  if (!dst)
    return Vec(src.X / 2.0, src.Y / 2.0);
  return Vec(src.X / 2.0 - dst.X() / 2.0,
    src.Y / 2.0 - dst.Y() / 2.0);
}

Vec center_point(const Vec src, const Vec dst)
  { return Vec(src.x / 2.0 - dst.x / 2.0, src.y / 2.0 - dst.y / 2.0); }

Image fast_cut(cr<Image> src, int sx, int sy, int mx, int my) {
  assert(src);
  assert(mx > 0);
  assert(my > 0);
  Image dst(mx, my);
  cauto ex {sx + mx};
  cauto ey {sy + my};

  #pragma omp parallel for simd collapse(2) if (dst.size >= 64 * 64)
  for (auto y = sy; y < ey; ++y)
  for (auto x = sx; x < ex; ++x)
    dst.fast_set(x - sx, y - sy, src(x, y), {});

  return dst;
}

void fast_cut_2(Image& dst, cr<Image> src, const int sx, const int sy, const int mx, const int my) noexcept {
  assert(src);
  assert(dst.size >= mx * my);
  assert(mx > 0);
  assert(my > 0);
  cauto ex {sx + mx};
  cauto ey {sy + my};

  #pragma omp parallel for simd collapse(2) if (dst.size >= 64 * 64)
  for (auto y = sy; y < ey; ++y)
  for (auto x = sx; x < ex; ++x)
    dst.fast_set(x - sx, y - sy, src(x, y), {});
}

Image cut(cr<Image> src, cr<Recti> rect, Image_get mode) {
  assert(src);
  return_if (rect.size.x <= 0 || rect.size.y <= 0, {});

  Image ret(rect.size.x, rect.size.y);
  cauto ex = rect.pos.x + rect.size.x;
  cauto ey = rect.pos.y + rect.size.y;
  for (auto y = rect.pos.y; y < ey; ++y)
  for (auto x = rect.pos.x; x < ex; ++x)
    ret.set(x - rect.pos.x, y - rect.pos.y, src.get(x, y, mode));
  return ret;
}

Sprite optimize_size(cr<Sprite> src, Vec& offset) {
  return_if (!src, Sprite{});
  
  crauto mask = src.mask();
  int sx {mask.X - 1};
  int ex {-1};
  int sy {mask.Y - 1};
  int ey {-1};

  // по наличию непрозрачного пикселя в маске определяется область для вырезания
  cfor (y, mask.Y)
  cfor (x, mask.X) {
    if (mask(x, y) != Pal8::mask_invisible) {
      if (x < sx)
        sx = x;
      if (x > ex)
        ex = x;
      if (y < sy)
        sy = y;
      if (y > ey)
        ey = y;
    }
  }

  // оффсет тоже обновляется
  offset.x += sx;
  offset.y += sy;

  Sprite ret;
  ret.image() = fast_cut(src.image(), sx, sy, ex - sx + 1, ey - sy + 1);
  ret.mask()  = fast_cut(mask,        sx, sy, ex - sx + 1, ey - sy + 1);
  return ret;
} // optimize_size

void insert_x2(Image& dst, cr<Image> src, Vec pos) {
  if (!dst || !src) {
    log_warning << "WARNING: insert_x2 dst or src is empty";
    return;
  }
  
  // увеличенный пребуфер
  auto srx_x_x2 {src.X * 2};
  auto srx_y_x2 {src.Y * 2};
  Image insert_x2_buf(srx_x_x2, srx_y_x2);

  cfor (y, src.Y)
  cfor (x, src.X) {
    auto col{ src(x, y) };
    insert_x2_buf.fast_set(x * 2 + 0, y * 2 + 0, col, {});
    insert_x2_buf.fast_set(x * 2 + 1, y * 2 + 0, col, {});
    insert_x2_buf.fast_set(x * 2 + 0, y * 2 + 1, col, {});
    insert_x2_buf.fast_set(x * 2 + 1, y * 2 + 1, col, {});
  }

  insert(dst, insert_x2_buf, pos);
} // insert_x2

Rect get_insertion_bound(cr<Image> dst, const Vec pos, cr<Image> src) noexcept {
  Rect bound( Vec{}, Vec(src.X, src.Y) );

  // уход за право/низ
  if (pos.x + bound.size.x >= dst.X)
    bound.size.x -= pos.x + bound.size.x - dst.X;
  if (pos.y + bound.size.y >= dst.Y)
    bound.size.y -= pos.y + bound.size.y - dst.Y;
  // уход за лево/верх
  if (pos.x < 0) {
    bound.pos.x = pos.x * -1;
    bound.size.x += pos.x; // pos отрицательный
  }
  if (pos.y < 0) {
    bound.pos.y = pos.y * -1;
    bound.size.y += pos.y; // pos отрицательный
  }

  using T = decltype(bound.size.x);
  bound.pos.x  = std::clamp<T>(bound.pos.x,  0, src.X);
  bound.pos.y  = std::clamp<T>(bound.pos.y,  0, src.Y);
  bound.size.x = std::clamp<T>(bound.size.x, 0, src.X - bound.pos.x);
  bound.size.y = std::clamp<T>(bound.size.y, 0, src.Y - bound.pos.y);
  return bound;
} // insert_bound

void insert(Image& dst, cr<Sprite> src, Vec pos, blend_pf bf,
int optional) {
  if (!src || !dst) {
    log_warning << "WARNING: src or dst is empty";
    return;
  }
  crauto src_image = src.image();
  crauto src_mask = src.mask();

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
  int dst_front_porch = std::max<real>(0, pos.x);
  int dst_back_porch = (dst.X - (ex - sx)) - std::max<real>(0, pos.x);
  int src_front_porch = bound.pos.x;
  int src_back_porch = src_image.X - (bound.pos.x + bound.size.x);

  // начальное смещение src по y
  src_p += sy * src_image.X;
  mask_p += sy * src_image.X;
  // начальное смещение dst по y с оффсетом
  dst_p += (sy + scast<int>(pos.y)) * dst.X;

  for (auto y = sy; y < ey; ++y) {
    // смещение по x (левый пич)
    dst_p += dst_front_porch;
    mask_p += src_front_porch;
    src_p += src_front_porch;

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
} // insert image sprite bf

void blend(Image& dst, cr<Image> src, const Vec pos, real alpha,
blend_pf bf, int optional) {
  error("test it");
  return_if (!dst || !src);
  return_if (alpha <= 0);
  if (alpha >= 1) {
    insert(dst, src, pos, bf, optional);
    return;
  }
  cfor (y, src.Y)
  cfor (x, src.X) {
    Pal8 a = dst.get(x + pos.x, y + pos.y);
    const Pal8 b = src(x, y);
    auto ar = b.to_real();
    auto br = a.to_real();
    // ret = dst + (src - dst) * alpha
    auto yr = std::clamp<real>(br + (ar - br) * alpha, 0, 1);
    auto past_color = Pal8::from_real(yr, a.is_red() || b.is_red());
    dst.set(x + pos.x, y + pos.y, past_color, bf, optional);
  }
}

void blend(Image& dst, cr<Sprite> src, const Vec pos, real alpha,
blend_pf bf, int optional) {
  return_if (!src || !dst);
  return_if (alpha <= 0);
  if (alpha >= 1) {
    insert(dst, src, pos, bf, optional);
    return;
  }
  auto &mask = src.mask();
  auto &image = src.image();
  cfor (y, src.Y())
  cfor (x, src.X()) {
    Pal8 a = dst.get(x + pos.x, y + pos.y);
    continue_if (mask(x, y) == Pal8::mask_invisible);
    const Pal8 b = image(x, y);
    auto ar = b.to_real();
    auto br = a.to_real();
    // ret = dst + (src - dst) * alpha
    auto yr = std::clamp<real>(br + (ar - br) * alpha, 0, 1);
    auto past_color = Pal8::from_real(yr, a.is_red() || b.is_red());
    dst.set(x + pos.x, y + pos.y, past_color, bf, optional);
  }
}

void draw_polygon(Image& dst, const Vec pos, cr<Polygon> poly,
const Pal8 color) {
  auto max_points = poly.points.size();
  cfor (i, max_points) {
    auto p1 = (pos + poly.offset + poly.points[i]);
    auto p2 = (pos + poly.offset + poly.points[(i + 1) % max_points]);
    draw_line(dst, ceil(p1), ceil(p2), color);
  }
}

void add_brightness(Image& dst, const Pal8 brightness) noexcept {
  return_if (brightness == 0);

  #pragma omp parallel for simd if (dst.size >= 64 * 64)
  cfor (i, dst.size)
    dst[i] = blend_sub_safe(brightness, dst[i]);
}

void sub_brightness(Image& dst, const Pal8 brightness) noexcept {
  return_if (brightness == 0);

  #pragma omp parallel for simd if (dst.size >= 64 * 64)
  cfor (i, dst.size)
    dst[i] = blend_sub_safe(brightness, dst[i]);
}

void apply_invert(Image& dst) noexcept {
  #pragma omp parallel for simd if (dst.size >= 64 * 64)
  cfor (i, dst.size)
    dst[i].apply_invert();
}

void to_red(Image& dst) {
  for (rauto pix: dst)
    pix = Pal8::from_real(pix.to_real(), true);
}

void to_gray_accurate(Image& dst, cr<Image> src) noexcept {
  assert(src);
  assert(dst);
  assert(src.size == dst.size);

  #pragma omp parallel for simd if (dst.size >= 64 * 64)
  cfor (i, src.size) {
    cauto pix = src[i];
    cauto is_red = pix.is_red();
  
    if (is_red)
      dst[i] = Pal8::from_real(pix.to_real() * 0.2989f, false);
    elif (pix == Pal8::white)
      dst[i] = Pal8::gray_end;
    else // is gray
      dst[i] = pix;
  }
}

void to_gray(Image& dst) {
  for (rauto pix: dst)
    pix = Pal8::from_real(pix.to_real(), false);
}

void expand_color_4(Image& dst, const Pal8 color) {
  Image buffer(dst);
  cpauto buffer_ptr = buffer.data();

  cfor (y, dst.Y)
  cfor (x, dst.X) {
    if (*buffer_ptr == color) {
      dst.set(x+0, y-1, color);
      dst.set(x-1, y+0, color);
      dst.set(x+1, y+0, color);
      dst.set(x+0, y+1, color);
    }
    ++buffer_ptr;
  }
}

void expand_color_8(Image& dst, const Pal8 color) {
  Image buffer(dst);
  cpauto buffer_ptr = buffer.data();

  cfor (y, dst.Y)
  cfor (x, dst.X) {
    if (*buffer_ptr == color) {
      dst.set(x-1, y-1, color);
      dst.set(x+0, y-1, color);
      dst.set(x+1, y-1, color);
      dst.set(x-1, y+0, color);
      dst.set(x+1, y+0, color);
      dst.set(x-1, y+1, color);
      dst.set(x+0, y+1, color);
      dst.set(x+1, y+1, color);
    }
    ++buffer_ptr;
  }
}

void expand_color_4_buf(Image& dst, Image& tmp, const Pal8 color) {
  assert(dst.size >= tmp.size);
  cpauto tmp_ptr = tmp.data();

  cfor (y, dst.Y)
  cfor (x, dst.X) {
    if (*tmp_ptr == color) {
      dst.set(x+0, y-1, color);
      dst.set(x-1, y+0, color);
      dst.set(x+1, y+0, color);
      dst.set(x+0, y+1, color);
    }
    ++tmp_ptr;
  }
}

void expand_color_8_buf(Image& dst, Image& tmp, const Pal8 color) {
  assert(dst.size >= tmp.size);
  cpauto tmp_ptr = tmp.data();

  cfor (y, dst.Y)
  cfor (x, dst.X) {
    if (*tmp_ptr == color) {
      dst.set(x-1, y-1, color);
      dst.set(x+0, y-1, color);
      dst.set(x+1, y-1, color);
      dst.set(x-1, y+0, color);
      dst.set(x+1, y+0, color);
      dst.set(x-1, y+1, color);
      dst.set(x+0, y+1, color);
      dst.set(x+1, y+1, color);
    }
    ++tmp_ptr;
  }
}

void insert_blured(Image& dst, cr<Sprite> src, Vec old_pos, Vec cur_pos, blend_pf bf, Uid uid) {
  old_pos = floor(old_pos);
  cur_pos = floor(cur_pos);
  cauto traveled = distance(old_pos, cur_pos);
  // если мало прошли, то не блюрить
  const bool small_len = traveled <= graphic::insert_blured_traveled_limit;
  // условие для выключение блюра
  bool disable_blur = false;
  if (graphic::motion_blur_mode == Motion_blur_mode::disabled) // выключили в настройках
    disable_blur = true;
  else if (graphic::motion_blur_mode == Motion_blur_mode::autoopt) // для мигания при автооптимизации
    disable_blur = graphic::render_lag && (uid + graphic::frame_count) & 1;
  
  if (small_len || disable_blur) {
    insert(dst, src, cur_pos, bf, uid);
    return;
  }

  auto motion_blur_quality_mul = graphic::motion_blur_quality_mul;

  // сменить качество блюра при автооптимизации
  if (graphic::motion_blur_mode == Motion_blur_mode::autoopt && graphic::render_lag)
    motion_blur_quality_mul = std::max(graphic::max_motion_blur_quality_reduct, graphic::motion_blur_quality_mul);

  Vec pos = old_pos;
  Vec step = normalize_graphic(cur_pos - old_pos);
  int blur_len = std::floor( safe_div(traveled, motion_blur_quality_mul) );
  cfor (i, blur_len) {
    insert(dst, src, pos, bf, uid);
    pos += step * motion_blur_quality_mul;
  }
  insert(dst, src, pos, bf, uid);
  insert(dst, src, cur_pos, bf, uid);

} // insert_blured

blend_pf find_blend_f(cr<Str> name) {
  static const std::unordered_map<Str, blend_pf> table {
    {"blend_none", &blend_none},
    {"blend_rotate", &blend_rotate},
    {"blend_rotate_x4", &blend_rotate_x4},
    {"blend_rotate_x16", &blend_rotate_x16},
    {"blend_rotate_safe", &blend_rotate_safe},
    {"blend_rotate_x4_safe", &blend_rotate_x4_safe},
    {"blend_rotate_x16_safe", &blend_rotate_x16_safe},
    {"blend_past", &blend_past},
    {"blend_or", &blend_or},
    {"blend_sub", &blend_sub},
    {"blend_add", &blend_add},
    {"blend_mul", &blend_mul},
    {"blend_and", &blend_and},
    {"blend_min", &blend_min},
    {"blend_max", &blend_max},
    {"blend_avr", &blend_avr},
    {"blend_avr_max", &blend_avr_max},
    {"blend_158", &blend_158},
    {"blend_diff", &blend_diff},
    {"blend_xor", &blend_xor},
    {"blend_xor_safe", &blend_xor_safe},
    {"blend_overlay", &blend_overlay},
    {"blend_or_safe", &blend_or_safe},
    {"blend_add_safe", &blend_add_safe},
    {"blend_sub_safe", &blend_sub_safe},
    {"blend_mul_safe", &blend_mul_safe},
    {"blend_and_safe", &blend_and_safe},
    {"blend_softlight", &blend_softlight},
    {"blend_no_black", &blend_no_black},
    {"blend_diff_no_black", &blend_diff_no_black},
  };

  try {
    return table.at(name);
  } catch (...) {
    error("blend_f по имени " + name + " не найден");
  }

  return {};
} // insert_blured

void apply_contrast(Image& dst, real contrast) noexcept {
  assert(contrast >= 0);

  #pragma omp parallel for simd if (dst.size > 64 * 64)
  cfor (i, dst.size) {
    auto& pix = dst[i];
    cauto is_red = pix.is_red();
    auto dst = (pix.to_real() - 0.5f) * contrast;
    dst += 0.5f;
    pix = Pal8::from_real(dst, is_red);
  }
}

void apply_brightness(Image& dst, const int val) noexcept {
  assert(dst);
  
  if (val < 0) {
    for (rauto pix: dst)
      pix.sub(val * -1);
  } else {
    for (rauto pix: dst)
      pix.add(val);
  }
}

void draw_spline(Image& dst, const Vec a, const Vec b,
const Vec c, const Vec d, const Pal8 color, const blend_pf bf,
const uint quality) {
  assert(dst);
  assert(quality > 0);
  assert(quality <= 10'000);

  const real a3 = (-a.x + 3.f * (b.x - c.x) + d.x) / 6.f;
  const real b3 = (-a.y + 3.f * (b.y - c.y) + d.y) / 6.f;
  const real a2 = (a.x - 2.f * b.x + c.x) / 2.f;
  const real b2 = (a.y - 2.f * b.y + c.y) / 2.f;
  const real a1 = (c.x - a.x) / 2.f;
  const real b1 = (c.y - a.y) / 2.f;
  const real a0 = (a.x + 4.f * b.x + c.x) / 6.f;
  const real b0 = (a.y + 4.f * b.y + c.y) / 6.f;

  for (uint i = 1; i < quality + 1; ++i) {
    cauto t0 = scast<real>(i - 1) / quality;
    cauto t1 = scast<real>(i) / quality;
    const Vec pos_0 (
      ((a3 * t0 + a2) * t0 + a1) * t0 + a0,
      ((b3 * t0 + b2) * t0 + b1) * t0 + b0
    );
    const Vec pos_1 (
      ((a3 * t1 + a2) * t1 + a1) * t1 + a0,
      ((b3 * t1 + b2) * t1 + b1) * t1 + b0
    );
    draw_line(dst, pos_0, pos_1, color, bf);
  }
} // draw_spline

void draw_line(Image& dst, Vec _p1, const Vec _p2,
const Pal8 color, const blend_pf bf) {
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
        dst.set(j >> 16, p1.y, color, bf, {});
        j += decInc;
      }
      return;
    }
    longLen += p1.y;
    for (int j = 0x8000 + (p1.x << 16); p1.y >= longLen; --p1.y) {
      dst.set(j >> 16, p1.y, color, bf, {});	
      j -= decInc;
    }
    return;	
  }
  if (longLen > 0) {
    longLen += p1.x;
    for (int j = 0x8000 + (p1.y << 16); p1.x <= longLen; ++p1.x) {
      dst.set(p1.x, j >> 16, color, bf, {});
      j += decInc;
    }
    return;
  }
  longLen += p1.x;
  for (int j = 0x8000 + (p1.y << 16); p1.x >= longLen; --p1.x) {
    dst.set(p1.x, j >> 16, color, bf, {});
    j -= decInc;
  }
} // draw_line

void draw_rect_filled(Image& dst, cr<Rect> rect, const Pal8 col, blend_pf bf, const int optional) {
  // проверить валидность
  return_if( !dst);
  return_if (rect.size.x < 1 || rect.size.y < 1);

  // определить границы прямоугольника
  int rect_sx = std::floor(rect.pos.x);
  int rect_sy = std::floor(rect.pos.y);
  int rect_ex = std::floor(rect.pos.x + rect.size.x);
  int rect_ey = std::floor(rect.pos.y + rect.size.y);

  // оптимизировать границы
  rect_sx = std::max(rect_sx, 0);
  rect_sy = std::max(rect_sy, 0);
  rect_ex = std::min(rect_ex, dst.X);
  rect_ey = std::min(rect_ey, dst.Y);
  cauto RECT_SZ_X = rect_ex - rect_sx;
  cauto RECT_SZ_Y = rect_ey - rect_sy;

  // не рисовать, если есть проблемы с размером
  return_if(RECT_SZ_X <= 0);
  return_if(RECT_SZ_Y <= 0);

  auto* dst_ptr = &dst(rect_sx, rect_sy);
  const std::size_t DST_PITCH = dst.X - RECT_SZ_X;

  cfor (y, RECT_SZ_Y) {
    cfor (x, RECT_SZ_X) {
      *dst_ptr = bf(col, *dst_ptr, optional);
      ++dst_ptr;
    }

    dst_ptr += DST_PITCH;
  }
}

void draw_rect(Image& dst, cr<Rect> rect, const Pal8 col, blend_pf bf, const int optional) noexcept {
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
    dst.set(x, rect_pos_y, col, bf, optional);
    dst.set(x, ey-1, col, bf, optional);
  }
  for (int y = rect_pos_y+1; y < ey-1; ++y) {
    dst.set(rect_pos_x, y, col, bf, optional);
    dst.set(ex-1, y, col, bf, optional);
  }
}
