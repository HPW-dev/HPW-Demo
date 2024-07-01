#ifndef __clang__
#include <omp.h>
#endif

#include <algorithm>
#include "plugin/graphic-effect/hpw-plugin-effect.h"
#include "pge-util.hpp"
#include "util/macro.hpp"
#include "util/vector-types.hpp"
#include "graphic/image/color.hpp"
#include "graphic/util/convert.hpp"

NOT_EXPORTED Pal8* g_dst {}; // ссыль на растр от игры
NOT_EXPORTED Vector<Pal8> g_buffer {};
NOT_EXPORTED uint16_t g_w {}; // ширина растра
NOT_EXPORTED uint16_t g_h {}; // высота растра
NOT_EXPORTED real_t g_fading {0.11f}; // как быстро затухает картинка

extern "C" EXPORTED void PLUG_CALL plugin_init(const struct context_t* context,
struct result_t* result) {
  // описание плагина
  result->full_name = "Fading";
  result->author = "HPW-dev";
  result->description = "Light fading effect (same as motion blur)";
  // проверка валидности данных
  if ( !check_params(context, result))
    return;
  // бинд параметров
  static_assert(sizeof(pal8_t) == sizeof(Pal8));
  g_dst = ptr2ptr<Pal8*>(context->dst);
  g_h = context->h;
  g_w = context->w;
  g_buffer = decltype(g_buffer)(g_w * g_h, Pal8(Pal8::black));
  if (g_fading <= 0.0f && g_fading >= 1.0f) {
    result->error = "out of range for fading value (0..1)";
    result->init_succsess = false;
    return;
  }
  context->registrate_param_f32("Fading speed", "Light fading speed",
    &g_fading, 0.001, 0.001, 0.5);
} // plugin_init

extern "C" EXPORTED void PLUG_CALL plugin_apply(uint32_t state) {
  #pragma omp parallel for simd
  cfor (i, g_w * g_h) {
    nauto buffer_pix = g_buffer[i];
    auto buffer_rgb = to_rgb24(buffer_pix);
    nauto dst_pix = g_dst[i];
    cauto dst_rgb = to_rgb24(dst_pix);
    // затухание
    buffer_rgb.r = std::max<real_t>(0, scast<real_t>(buffer_rgb.r) - (g_fading * 255.0) );
    buffer_rgb.g = std::max<real_t>(0, scast<real_t>(buffer_rgb.g) - (g_fading * 255.0) );
    buffer_rgb.b = std::max<real_t>(0, scast<real_t>(buffer_rgb.b) - (g_fading * 255.0) );
    // выбрать самый яркий пиксель
    buffer_rgb.r = std::max(dst_rgb.r, buffer_rgb.r);
    buffer_rgb.g = std::max(dst_rgb.g, buffer_rgb.g);
    buffer_rgb.b = std::max(dst_rgb.b, buffer_rgb.b);
    dst_pix = desaturate_bt601(buffer_rgb.r, buffer_rgb.g, buffer_rgb.b);
    buffer_pix = desaturate_bt601(buffer_rgb.r, buffer_rgb.g, buffer_rgb.b);
  }
} // plugin_apply

extern "C" EXPORTED void PLUG_CALL plugin_finalize(void) {
  g_buffer.clear();
}
