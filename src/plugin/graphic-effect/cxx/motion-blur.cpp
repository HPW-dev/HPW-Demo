#include <omp.h>
#include "plugin/graphic-effect/hpw-plugin-effect.h"
#include <cassert>
#include <algorithm>
#include "pge-util.hpp"
#include "util/macro.hpp"
#include "graphic/image/image.hpp"
#include "graphic/image/color.hpp"
#include "graphic/util/convert.hpp"

Pal8* g_dst {}; // ссыль на растр от игры
uint16_t g_w {}; // ширина растра
uint16_t g_h {}; // высота растра
real_t g_motion_blur; /// как сильно надо размывать кадры
Image g_old_frame; /// предыдущий кадр

extern "C" void plugin_init(const struct context_t* context, struct result_t* result) {
  // описание плагина
  result->full_name = "Brightness";
  result->description = "Add brightness";
  // проверка валидности данных
  if ( !check_params(context, result))
    return;
  // бинд параметров
  static_assert(sizeof(pal8_t) == sizeof(Pal8));
  g_dst = ptr2ptr<Pal8*>(context->dst);
  g_h = context->h;
  g_w = context->w;
  g_motion_blur = 0.055;
  context->registrate_param_f32(
    "power",
    "power of motion blur",
    &g_motion_blur, 0.05, 0.1, 0.8
  );
  // создание буфера предыдущего кадра
  g_old_frame.init(g_w, g_h);
} // plugin_init

extern "C" void plugin_apply(uint32_t state) {
  const real_t dst_ratio = g_motion_blur;
  const real_t old_frame_ratio = 1.0 - g_motion_blur;
  assert(old_frame_ratio > 0.0 && old_frame_ratio < 1.0);
  assert(dst_ratio > 0.0 && dst_ratio < 1.0);

  #pragma omp parallel for simd schedule(static, 4)
  cfor (i, g_w * g_h) {
    auto rgb_dst = to_rgb24(g_dst[i]);
    cauto rgb_old = to_rgb24(g_old_frame[i]);
    rgb_dst.r = rgb_dst.r * dst_ratio + rgb_old.r * old_frame_ratio;
    rgb_dst.g = rgb_dst.g * dst_ratio + rgb_old.g * old_frame_ratio;
    rgb_dst.b = rgb_dst.b * dst_ratio + rgb_old.b * old_frame_ratio;
    g_dst[i] = desaturate_bt601(rgb_dst.r, rgb_dst.g, rgb_dst.b);
    g_old_frame[i] = g_dst[i];
  }
}

extern "C" void plugin_finalize(void) { g_old_frame.free(); }
