#ifndef __clang__
#include <omp.h>
#endif

#include <cstring>
#include <algorithm>
#include "plugin/graphic-effect/hpw-plugin-effect.h"
#include "pge-util.hpp"
#include "util/macro.hpp"
#include "util/vector-types.hpp"
#include "graphic/image/color.hpp"
#include "graphic/util/convert.hpp"

NOT_EXPORTED Pal8* g_dst {}; // ссыль на растр от игры
NOT_EXPORTED uint16_t g_w {}; // ширина растра
NOT_EXPORTED uint16_t g_h {}; // высота растра
NOT_EXPORTED real_t g_power {1.0}; // резкость
NOT_EXPORTED Vector<Pal8> g_buffer {};

extern "C" EXPORTED void PLUG_CALL plugin_init(const struct context_t* context,
struct result_t* result) {
  // описание плагина
  result->full_name = "Sharpen";
  result->author = "HPW-dev";
  result->description = "Image sharpenizer";
  // проверка валидности данных
  if ( !check_params(context, result))
    return;
  // бинд параметров
  static_assert(sizeof(pal8_t) == sizeof(Pal8));
  g_dst = ptr2ptr<Pal8*>(context->dst);
  g_h = context->h;
  g_w = context->w;
  // создание строки в меню для управления параметром яркости
  context->registrate_param_f32(
    "power",
    "Sharpen power",
    &g_power,
    0.1f, 0.0f, 10.0f
  );
  g_buffer.resize(g_h * g_w);
} // plugin_init

extern "C" EXPORTED void PLUG_CALL plugin_apply(uint32_t state) {
  // скопировать кадр в буффер
  std::memcpy(g_buffer.data(), g_dst, sizeof(Pal8) * g_buffer.size());
  constexpr float sin45d = 0.70710678118;
  constexpr float center = sin45d * 4.0 + 4.0 + 1;

  #pragma omp parallel for simd schedule(static) collapse(2)
  for (int y = 1; y < g_h - 1; ++y)
  for (int x = 1; x < g_w - 1; ++x) {
    cauto p00 = to_rgb24( get_pixel_fast(g_buffer.data(), x-1, y-1, g_w) );
    cauto p10 = to_rgb24( get_pixel_fast(g_buffer.data(), x+0, y-1, g_w) );
    cauto p20 = to_rgb24( get_pixel_fast(g_buffer.data(), x+1, y-1, g_w) );
    cauto p01 = to_rgb24( get_pixel_fast(g_buffer.data(), x-1, y+0, g_w) );
    cauto p11 = to_rgb24( get_pixel_fast(g_buffer.data(), x+0, y+0, g_w) );
    cauto p21 = to_rgb24( get_pixel_fast(g_buffer.data(), x+1, y+0, g_w) );
    cauto p02 = to_rgb24( get_pixel_fast(g_buffer.data(), x-1, y+1, g_w) );
    cauto p12 = to_rgb24( get_pixel_fast(g_buffer.data(), x+0, y+1, g_w) );
    cauto p22 = to_rgb24( get_pixel_fast(g_buffer.data(), x+1, y+1, g_w) );
    const float r =
      p00.r * g_power * -sin45d + p10.r * g_power * -1     + p20.r * g_power * -sin45d +
      p01.r * g_power * -1      + p11.r * g_power * center + p21.r * g_power * -1 +
      p02.r * g_power * -sin45d + p12.r * g_power * -1     + p22.r * g_power * -sin45d
    ;
    const float g =
      p00.g * g_power * -sin45d + p10.g * g_power * -1     + p20.g * g_power * -sin45d +
      p01.g * g_power * -1      + p11.g * g_power * center + p21.g * g_power * -1 +
      p02.g * g_power * -sin45d + p12.g * g_power * -1     + p22.g * g_power * -sin45d
    ;
    const float b =
      p00.b * g_power * -sin45d + p10.b * g_power * -1     + p20.b * g_power * -sin45d +
      p01.b * g_power * -1      + p11.b * g_power * center + p21.b * g_power * -1 +
      p02.b * g_power * -sin45d + p12.b * g_power * -1     + p22.b * g_power * -sin45d
    ;
    set_pixel_fast(g_dst, x, y, g_w, desaturate_bt601(
      std::clamp<float>(r, 0, 255),
      std::clamp<float>(g, 0, 255),
      std::clamp<float>(b, 0, 255)
    ));
  } // for y x
} // plugin_apply

extern "C" EXPORTED void PLUG_CALL plugin_finalize(void) {
  g_buffer.clear();
}
