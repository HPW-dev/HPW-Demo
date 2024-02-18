#include <omp.h>
#include <algorithm>
#include "plugin/graphic-effect/hpw-plugin-effect.h"
#include "pge-util.hpp"
#include "util/macro.hpp"
#include "graphic/image/image.hpp"
#include "graphic/image/color.hpp"
#include "graphic/util/convert.hpp"

Pal8* g_dst {}; // ссыль на растр от игры
uint16_t g_w {}; // ширина растра
uint16_t g_h {}; // высота растра
int32_t g_blend {0}; // режим смешивания (0 - среднее, 1 - макс, 2 - сосед)

extern "C" void plugin_init(const struct context_t* context, struct result_t* result) {
  // описание плагина
  result->full_name = "Pixelate";
  result->description = "Pixel sizeup";
  // проверка валидности данных
  if ( !check_params(context, result))
    return;
  // бинд параметров
  static_assert(sizeof(pal8_t) == sizeof(Pal8));
  g_dst = ptr2ptr<Pal8*>(context->dst);
  g_h = context->h;
  g_w = context->w;
  context->registrate_param_i32(
    "blend mode",
    "blend modes:\n"
    "  0 - average;\n"
    "  1 - max;\n"
    "  2 - neighbor.",
    &g_blend, 1, 0, 2
  );
} // plugin_init

extern "C" void plugin_apply(uint32_t state) {
  switch (g_blend) {
    default:
    case 0: { // average
      #pragma omp parallel for simd schedule(static, 4)
      for (int y = 0; y < g_h - 1; y += 2)
      for (int x = 0; x < g_w - 1; x += 2) {
        cauto a = to_rgb24( get_pixel_fast(g_dst, x+0, y+0, g_w) );
        cauto b = to_rgb24( get_pixel_fast(g_dst, x+1, y+0, g_w) );
        cauto c = to_rgb24( get_pixel_fast(g_dst, x+0, y+1, g_w) );
        cauto d = to_rgb24( get_pixel_fast(g_dst, x+1, y+1, g_w) );
        cauto avr = desaturate_bt601(
          (a.r + b.r + c.r + d.r) / 4u,
          (a.g + b.g + c.g + d.g) / 4u,
          (a.b + b.b + c.b + d.b) / 4u
        );
        set_pixel_fast(g_dst, x+0, y+0, g_w, avr);
        set_pixel_fast(g_dst, x+1, y+0, g_w, avr);
        set_pixel_fast(g_dst, x+0, y+1, g_w, avr);
        set_pixel_fast(g_dst, x+1, y+1, g_w, avr);
      }
      break;
    }

    case 1: { // max
      #pragma omp parallel for simd schedule(static, 4)
      for (int y = 0; y < g_h - 1; y += 2)
      for (int x = 0; x < g_w - 1; x += 2) {
        cauto a = to_rgb24( get_pixel_fast(g_dst, x+0, y+0, g_w) );
        cauto b = to_rgb24( get_pixel_fast(g_dst, x+1, y+0, g_w) );
        cauto c = to_rgb24( get_pixel_fast(g_dst, x+0, y+1, g_w) );
        cauto d = to_rgb24( get_pixel_fast(g_dst, x+1, y+1, g_w) );
        cauto max = desaturate_bt601(
          std::max( std::max(a.r, b.r), std::max(c.r, d.r) ),
          std::max( std::max(a.g, b.g), std::max(c.g, d.g) ),
          std::max( std::max(a.b, b.b), std::max(c.b, d.b) )
        );
        set_pixel_fast(g_dst, x+0, y+0, g_w, max);
        set_pixel_fast(g_dst, x+1, y+0, g_w, max);
        set_pixel_fast(g_dst, x+0, y+1, g_w, max);
        set_pixel_fast(g_dst, x+1, y+1, g_w, max);
      }
      break;
    }

    case 2: { // neighbor
      #pragma omp parallel for simd schedule(static, 4)
      for (int y = 0; y < g_h - 1; y += 2)
      for (int x = 0; x < g_w - 1; x += 2) {
        cauto pix = get_pixel_fast(g_dst, x, y, g_w);
        set_pixel_fast(g_dst, x+1, y+0, g_w, pix);
        set_pixel_fast(g_dst, x+0, y+1, g_w, pix);
        set_pixel_fast(g_dst, x+1, y+1, g_w, pix);
      }
      break;
    }
  }
} // plugin_apply

extern "C" void plugin_finalize(void) {}
