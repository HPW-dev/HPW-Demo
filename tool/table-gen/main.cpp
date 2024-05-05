#include <cassert>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <utility>
#include <string>
#include "util/str.hpp"
#include "util/file/file.hpp"
#include "util/macro.hpp"
#include "graphic/image/color.hpp"
#include "graphic/util/convert.hpp"

using Core_func_1d = std::function<Pal8 (int)>;
using Core_func_2d = std::function<Pal8 (int, int)>;
using Core_func_2d_opt = std::function<Pal8 (int, int, int)>;

//#define desaturate_f desaturate_average
#define desaturate_f desaturate_bt601

// генерит 1D таблицу
void gen_table_1d(CN<Str> table_name, Core_func_1d core) {
  static_assert(sizeof(Pal8::value_t) == sizeof(Bytes::value_type));
  Bytes data;
  data.reserve(256);
  cfor (i, 256)
    data.emplace_back(core(i).val);
  mem_to_file(data, table_name);
  std::cout << "complete \"" << table_name << "\"" << std::endl;
}

// генерит 2D таблицу
void gen_table_2d(CN<Str> table_name, Core_func_2d core) {
  static_assert(sizeof(Pal8::value_t) == sizeof(Bytes::value_type));
  Bytes data;
  data.reserve(256*256);
  cfor (y, 256)
    cfor (x, 256)
      data.emplace_back(core(x, y).val);
  mem_to_file(data, table_name);
  std::cout << "complete \"" << table_name << "\"" << std::endl;
}

// генерит 2D таблицу с учётом опционального параметра
void gen_table_2d_opt(CN<Str> table_name, Core_func_2d_opt core) {
  static_assert(sizeof(Pal8::value_t) == sizeof(Bytes::value_type));
  Bytes data;
  data.reserve(256*256*256);
  cfor (opt, 256)
    cfor (y, 256)
      cfor (x, 256)
        data.emplace_back(core(x, y, opt).val);
  mem_to_file(data, table_name);
  std::cout << "complete \"" << table_name << "\"" << std::endl;
} // gen_table_2d_opt

int main(int argc, char *argv[]) {
  gen_table_1d("table_inv.dat", [](int i)->Pal8 { return ~Pal8(i); } );

  gen_table_1d("table_dec_safe.dat", [](int i)->Pal8 {
    Pal8 col(i);
    if (col == Pal8::black)
      return col;
    if (col.is_red())
      col.val = std::clamp<int>(scast<int>(col.val) - 1, Pal8::red_black, Pal8::red_end);
    else
      col.val = std::clamp<int>(scast<int>(col.val) - 1, Pal8::black, Pal8::gray_end);
    return col;
  } );

  gen_table_1d("table_inc_safe.dat", [](int i)->Pal8 {
    Pal8 col(i);
    if (col == Pal8::white)
      return col;
    if (col.is_red())
      col.val = std::clamp<int>(scast<int>(col.val) + 1, Pal8::red_black, Pal8::red_end);
    else
      col.val = std::clamp<int>(scast<int>(col.val) + 1, Pal8::black, Pal8::gray_end);
    return col;
  } );

  gen_table_1d("table_inv_safe.dat", [](int i)->Pal8 {
    Pal8 col(i);
    if (col.is_white())
      return Pal8::black;
    auto cr = col.to_real();
    Pal8::value_t cu = std::clamp<uint>(cr * 255, 0, 255);
    cu = ~cu;
    cr = cu / 255.0;
    return Pal8::from_real(cr, col.is_red());
  });

  gen_table_2d("table_add.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    return Pal8(a.val + b.val);
  });

  gen_table_2d("table_add_safe.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    const Rgb24 dst_rgb (
      int(a_rgb.r) + b_rgb.r,
      int(a_rgb.g) + b_rgb.g,
      int(a_rgb.b) + b_rgb.b
    );
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  gen_table_2d("table_sub.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    return Pal8(a.val - b.val);
  });

  gen_table_2d("table_sub_safe.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    const Rgb24 dst_rgb (
      int(a_rgb.r) - b_rgb.r,
      int(a_rgb.g) - b_rgb.g,
      int(a_rgb.b) - b_rgb.b
    );
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

   gen_table_2d("table_and.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    return Pal8(a.val & b.val);
  });

  gen_table_2d("table_and_safe.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    const Rgb24 dst_rgb (
      int(a_rgb.r) & b_rgb.r,
      int(a_rgb.g) & b_rgb.g,
      int(a_rgb.b) & b_rgb.b
    );
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  gen_table_2d("table_or.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    return Pal8(a.val | b.val);
  });

  gen_table_2d("table_or_safe.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    const Rgb24 dst_rgb (
      int(a_rgb.r) | b_rgb.r,
      int(a_rgb.g) | b_rgb.g,
      int(a_rgb.b) | b_rgb.b
    );
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  gen_table_2d("table_mul.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    return Pal8(a.val * b.val);
  });

  gen_table_2d("table_mul_safe.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    const Rgb24 dst_rgb (
      int(a_rgb.r) * b_rgb.r,
      int(a_rgb.g) * b_rgb.g,
      int(a_rgb.b) * b_rgb.b
    );
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  gen_table_2d("table_xor.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    return Pal8(a.val ^ b.val);
  });

  gen_table_2d("table_xor_safe.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    const Rgb24 dst_rgb (
      int(a_rgb.r) ^ b_rgb.r,
      int(a_rgb.g) ^ b_rgb.g,
      int(a_rgb.b) ^ b_rgb.b
    );
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  gen_table_2d("table_diff.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    const Rgb24 dst_rgb (
      std::abs(int(a_rgb.r) - b_rgb.r),
      std::abs(int(a_rgb.g) - b_rgb.g),
      std::abs(int(a_rgb.b) - b_rgb.b)
    );
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  gen_table_2d("table_avr.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    const Rgb24 dst_rgb (
      (int(a_rgb.r) + b_rgb.r) / 2,
      (int(a_rgb.g) + b_rgb.g) / 2,
      (int(a_rgb.b) + b_rgb.b) / 2
    );
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  gen_table_2d("table_avr_max.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    const Rgb24 dst_rgb (
      std::max<int>((int(a_rgb.r) + b_rgb.r) / 2, b_rgb.r),
      std::max<int>((int(a_rgb.g) + b_rgb.g) / 2, b_rgb.g),
      std::max<int>((int(a_rgb.b) + b_rgb.b) / 2, b_rgb.b)
    );
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  gen_table_2d("table_blend158.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    constexpr auto mul = 158.0 / 255.0;
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    const Rgb24 dst_rgb (
      a_rgb.r + (int(b_rgb.r) - a_rgb.r) * mul,
      a_rgb.g + (int(b_rgb.g) - a_rgb.g) * mul,
      a_rgb.b + (int(b_rgb.b) - a_rgb.b) * mul
    );
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  gen_table_2d("table_max.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    const Rgb24 dst_rgb (
      std::max(a_rgb.r, b_rgb.r),
      std::max(a_rgb.g, b_rgb.g),
      std::max(a_rgb.b, b_rgb.b)
    );
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  gen_table_2d("table_min.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    const Rgb24 dst_rgb (
      std::min(a_rgb.r, b_rgb.r),
      std::min(a_rgb.g, b_rgb.g),
      std::min(a_rgb.b, b_rgb.b)
    );
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  gen_table_2d("table_overlay.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    #define OVERLAY(A, B) ((A / 255.0) < 0.5f) ? \
      (2 * (A / 255.0) * (B / 255.0)) : \
      (1.0 - 2 * (1.0 - (A / 255.0)) * (1.0 - (B / 255.0)))
    const Rgb24 dst_rgb (
      int(std::clamp<real>(OVERLAY(a_rgb.r, b_rgb.r), 0, 1) * 255.0),
      int(std::clamp<real>(OVERLAY(a_rgb.g, b_rgb.g), 0, 1) * 255.0),
      int(std::clamp<real>(OVERLAY(a_rgb.b, b_rgb.b), 0, 1) * 255.0)
    );
    #undef OVERLAY
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  gen_table_2d("table_softlight.dat", [](int x, int y) {
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    #define SOFTLIGHT(A, B) (A / 255.0) + (2.0 * (B / 255.0) * ((A / 255.0) * (1.0 - (A / 255.0))))
    const Rgb24 dst_rgb (
      int(std::clamp<real>(SOFTLIGHT(a_rgb.r, b_rgb.r), 0, 1) * 255.0),
      int(std::clamp<real>(SOFTLIGHT(a_rgb.g, b_rgb.g), 0, 1) * 255.0),
      int(std::clamp<real>(SOFTLIGHT(a_rgb.b, b_rgb.b), 0, 1) * 255.0)
    );
    #undef SOFTLIGHT
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  gen_table_2d_opt("table_fade_out_max.dat", [](int x, int y, int optional) {
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    const Rgb24 sub_rgb (
      int(a_rgb.r) - optional,
      int(a_rgb.g) - optional,
      int(a_rgb.b) - optional
    );
    cauto sub_ret = to_rgb24( desaturate_f(sub_rgb.r, sub_rgb.g, sub_rgb.b) );
    const Rgb24 dst_rgb (
      std::max(sub_ret.r, b_rgb.r),
      std::max(sub_ret.g, b_rgb.g),
      std::max(sub_ret.b, b_rgb.b)
    );
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  gen_table_2d_opt("table_fade_in_max.dat", [](int x, int y, int optional) {
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    const Rgb24 sub_rgb (
      int(a_rgb.r) - (255 - optional),
      int(a_rgb.g) - (255 - optional),
      int(a_rgb.b) - (255 - optional)
    );
    cauto sub_ret = to_rgb24( desaturate_f(sub_rgb.r, sub_rgb.g, sub_rgb.b) );
    const Rgb24 dst_rgb (
      std::max(sub_ret.r, b_rgb.r),
      std::max(sub_ret.g, b_rgb.g),
      std::max(sub_ret.b, b_rgb.b)
    );
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  gen_table_2d_opt("table_blend_alpha.dat", [](int x, int y, int optional) {
    cauto alpha = optional / 255.0;
    Pal8 a(x);
    Pal8 b(y);
    cauto a_rgb = to_rgb24(a);
    cauto b_rgb = to_rgb24(b);
    // ret = bg + (in - bg) * alpha
    #define BLEND_ALPHA(A, B) (B) + ((A) - (B)) * alpha
    const Rgb24 dst_rgb (
      int(std::clamp<real>(BLEND_ALPHA(a_rgb.r, b_rgb.r), 0, 255.0)),
      int(std::clamp<real>(BLEND_ALPHA(a_rgb.g, b_rgb.g), 0, 255.0)),
      int(std::clamp<real>(BLEND_ALPHA(a_rgb.b, b_rgb.b), 0, 255.0))
    );
    #undef BLEND_ALPHA
    return desaturate_f(dst_rgb.r, dst_rgb.g, dst_rgb.b);
  });

  return EXIT_SUCCESS;
}
