#include <omp.h>
#include <cassert>
#include <array>
#include <cmath>
#include "plugin/graphic-effect/hpw-plugin-effect.h"
#include "pge-util.hpp"
#include "util/macro.hpp"
#include "util/vector-types.hpp"
#include "graphic/image/color.hpp"

NOT_EXPORTED Pal8* g_dst {}; // ссыль на растр от игры
NOT_EXPORTED uint16_t g_w {}; // ширина растра
NOT_EXPORTED uint16_t g_h {}; // высота растра
NOT_EXPORTED int32_t g_symbol_set {1}; // сет символов
NOT_EXPORTED int32_t g_symbol_set_old {};
NOT_EXPORTED int32_t g_downscale_mode {1}; // режим уменьшения блоков
NOT_EXPORTED int32_t g_downscale_mode_old {};

NOT_EXPORTED inline constexpr uint SET_1_SZ = 400;
NOT_EXPORTED inline constexpr uint SET_1_Y = 4;
NOT_EXPORTED inline constexpr uint SET_1_X = 100;
// figures 4x4
NOT_EXPORTED inline constexpr byte SET_1[] {
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 
  0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 
  0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 
  0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 
  0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
  0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 
  0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 
  0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
  0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff
}; // SET_1

NOT_EXPORTED inline constexpr uint SET_0_SZ = 435;
NOT_EXPORTED inline constexpr uint SET_0_Y = 5;
NOT_EXPORTED inline constexpr uint SET_0_X = 87;
// mini font 3x5
NOT_EXPORTED inline constexpr byte SET_0[] {
  0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 
  0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 
  0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 
  0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 
  0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 
  0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 
  0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 
  0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 
  0xff, 0xff, 0x00
};

NOT_EXPORTED inline constexpr uint SET_2_SZ = 32;
NOT_EXPORTED inline constexpr uint SET_2_Y = 2;
NOT_EXPORTED inline constexpr uint SET_2_X = 16;
// 2x2 blocks
NOT_EXPORTED inline constexpr byte SET_2[] {
  0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff,
  0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00
};

NOT_EXPORTED uint g_tile_x {};
NOT_EXPORTED uint g_tile_y {};
using Tile = Vector<Pal8>;
NOT_EXPORTED Vector<Tile> g_tiles {};
NOT_EXPORTED void reinit();
NOT_EXPORTED void convert_tile(const uint pos_x, const uint pos_y);

extern "C" EXPORTED void PLUG_CALL plugin_init(const struct context_t* context,
struct result_t* result) {
  // описание плагина
  result->full_name = "Symbols";
  result->author = "HPW-dev";
  result->description = "Convert pixel graphic to symbolyc map";
  // проверка валидности данных
  if ( !check_params(context, result))
    return;
  // бинд параметров
  static_assert(sizeof(pal8_t) == sizeof(Pal8));
  g_dst = ptr2ptr<Pal8*>(context->dst);
  g_h = context->h;
  g_w = context->w;
  context->registrate_param_i32(
    "Symbol set",
    "sets:\n"
    "0 - nums with chars 3x5\n"
    "1 - pattern 4x4\n"
    "2 - pattern 2x2",
    &g_symbol_set, 1, 0, 2
  );
  context->registrate_param_i32(
    "Downscale",
    "Downscale tiles to small bocks for more quality conversion?\n"
    "0 - none\n"
    "1 - block size 3x3\n"
    "2 - blocks 2x2",
    &g_downscale_mode, 1, 0, 2
  );
  g_symbol_set_old = g_symbol_set;
  g_downscale_mode_old = g_downscale_mode;
  reinit();
} // plugin_init

extern "C" EXPORTED void PLUG_CALL plugin_apply(uint32_t state) {
  // проверить что опции изменились и применить изменения
  if (g_symbol_set != g_symbol_set_old
  || g_downscale_mode != g_downscale_mode_old) {
    g_symbol_set_old = g_symbol_set;
    g_downscale_mode_old = g_downscale_mode;
    reinit();
  }

  #pragma omp parallel for collapse(2)
  cfor (y, g_h / g_tile_y)
  cfor (x, g_w / g_tile_x)
    convert_tile(x * g_tile_x, y * g_tile_y);
} // plugin_apply

extern "C" EXPORTED void PLUG_CALL plugin_finalize(void)
  { g_tiles.clear(); }

NOT_EXPORTED void reinit() {
  // определить какая сейчас таблица символов
  CP<Pal8> symbol_set;
  uint symbol_set_sz;
  uint symbol_set_x;
  uint symbol_set_y;
  switch (g_symbol_set) {
    default:
    case 0: {
      symbol_set = rcast<CP<Pal8>>(SET_0);
      symbol_set_sz = SET_0_SZ;
      symbol_set_x = SET_0_X;
      symbol_set_y = SET_0_Y;
      g_tile_x = 3;
      g_tile_y = 5;
      break;
    }
    case 1: {
      symbol_set = rcast<CP<Pal8>>(SET_1);
      symbol_set_sz = SET_1_SZ;
      symbol_set_x = SET_1_X;
      symbol_set_y = SET_1_Y;
      g_tile_x = 4;
      g_tile_y = 4;
      break;
    }
    case 2: {
      symbol_set = rcast<CP<Pal8>>(SET_2);
      symbol_set_sz = SET_2_SZ;
      symbol_set_x = SET_2_X;
      symbol_set_y = SET_2_Y;
      g_tile_x = 2;
      g_tile_y = 2;
      break;
    }
  } // switch (g_symbol_set)
  assert(g_tile_x > 0);
  assert(g_tile_y > 0);
  assert(symbol_set_x > 0);
  assert(symbol_set_y > 0);
  assert(symbol_set_sz > 0);
  assert(symbol_set_sz == symbol_set_x * symbol_set_y);

  // нарезать таблицу на тайлы
  g_tiles.clear();
  cauto tile_count = symbol_set_sz / (g_tile_x * g_tile_y);
  cfor (tile_idx, tile_count) {
    Tile tile(g_tile_x * g_tile_y);
    cfor (y, g_tile_y)
    cfor (x, g_tile_x) {
      cauto src_pix = get_pixel_safe(symbol_set, x + tile_idx * g_tile_x, y,
        symbol_set_x, symbol_set_y);
      set_pixel_safe(tile.data(), x, y, g_tile_x, g_tile_y, src_pix);
    }
    g_tiles.emplace_back(std::move(tile));
  }
} // reinit

NOT_EXPORTED real check_difference(CN<Tile> a, CN<Tile> b) {
  assert(a.size() == b.size());
  real difference {};
  cfor (i, a.size())
    difference += std::abs(a[i].to_real() - b[i].to_real());
  return difference;
}

NOT_EXPORTED inline constexpr real blerp(const real c00, const real c10, const real c01,
const real c11, const real tx, const real ty)
  { return std::lerp(std::lerp(c00, c10, tx), std::lerp(c01, c11, tx), ty); }

NOT_EXPORTED Tile scale_bilinear(CN<Tile> src,
const uint dst_x, const uint dst_y) {
  Tile dst(dst_x * dst_y);
  assert(!dst.empty());
  const real scale_x = 1.0 / (real(dst_x) / g_tile_x);
  const real scale_y = 1.0 / (real(dst_y) / g_tile_y);

  cfor (y, dst_y)
  cfor (x, dst_x) {
    const real dx = x * scale_x;
    const real dy = y * scale_y;
    const int gxi = std::floor<int>(dx);
    const int gyi = std::floor<int>(dy);
    cauto c00 = get_pixel_safe(src.data(), gxi,     gyi,     g_tile_x, g_tile_y).to_real();
    cauto c10 = get_pixel_safe(src.data(), gxi + 1, gyi,     g_tile_x, g_tile_y).to_real();
    cauto c01 = get_pixel_safe(src.data(), gxi,     gyi + 1, g_tile_x, g_tile_y).to_real();
    cauto c11 = get_pixel_safe(src.data(), gxi + 1, gyi + 1, g_tile_x, g_tile_y).to_real();
    cauto tx = dx - gxi;
    cauto ty = dy - gyi;
    cauto l = blerp(c00, c10, c01, c11, tx, ty);
    set_pixel_safe(dst.data(), x, y, dst_x, dst_y, Pal8::from_real(l));
  }
  return dst;
} // scale_bilinear

NOT_EXPORTED real check_difference_3x3(CN<Tile> a, CN<Tile> b) {
  assert(a.size() == b.size());
  real difference {};
  // уменьшить до 3x3 с билинейной интерполяцией
  cauto a_3x3 = scale_bilinear(a, 3, 3);
  cauto b_3x3 = scale_bilinear(b, 3, 3);
  cfor (i, a_3x3.size())
    difference += std::abs(a_3x3[i].to_real() - b_3x3[i].to_real());
  return difference;
}

NOT_EXPORTED real check_difference_2x2(CN<Tile> a, CN<Tile> b) {
  assert(a.size() == b.size());
  real difference {};
  // уменьшить до 2x2 с билинейной интерполяцией
  cauto a_2x2 = scale_bilinear(a, 2, 2);
  cauto b_2x2 = scale_bilinear(b, 2, 2);
  cfor (i, a_2x2.size())
    difference += std::abs(a_2x2[i].to_real() - b_2x2[i].to_real());
  return difference;
}

NOT_EXPORTED void convert_tile(const uint pos_x, const uint pos_y) {
  // вырезать тайл исходника
  Tile dst_tile(g_tile_x * g_tile_y);
  cfor (y, g_tile_y)
  cfor (x, g_tile_x) {
    auto src_pix = get_pixel_safe(g_dst, x + pos_x, y + pos_y, g_w, g_h);
    src_pix = src_pix.to_real() > 0.5 ? Pal8::white : Pal8::black;
    set_pixel_safe(dst_tile.data(), x, y, g_tile_x, g_tile_y, src_pix);
  }
  
  // определить какой тайл самый похожий на dst_tile
  real difference {9'999'999.0};
  uint result_tile_idx {};
  for (uint tile_idx {}; cnauto tile: g_tiles) {
    real local_difference;
    // нужно ли уменьшать размеры тайлов
    switch (g_downscale_mode) {
      default:
      case 0: local_difference = check_difference(dst_tile, tile); break;
      case 1: local_difference = check_difference_3x3(dst_tile, tile); break;
      case 2: local_difference = check_difference_2x2(dst_tile, tile); break;
    }
    // определять по наименьшей разнице
    if (local_difference < difference) {
      difference = local_difference;
      result_tile_idx = tile_idx;
    }
    ++tile_idx;
  }
  cnauto result_tile = g_tiles.at(result_tile_idx);

  cfor (y, g_tile_y)
  cfor (x, g_tile_x) {
    cauto src_pix = get_pixel_safe(result_tile.data(), x, y, g_tile_x, g_tile_y);
    set_pixel_safe(g_dst, x + pos_x, y + pos_y, g_w, g_h, src_pix);
  }
} // convert_tile