#include <omp.h>
#include <algorithm>
#include "plugin/graphic-effect/hpw-plugin-effect.h"
#include "pge-util.hpp"
#include "util/macro.hpp"
#include "graphic/image/color.hpp"
#include "graphic/util/convert.hpp"

NOT_EXPORTED Pal8* g_dst {}; // ссыль на растр от игры
NOT_EXPORTED uint16_t g_w {}; // ширина растра
NOT_EXPORTED uint16_t g_h {}; // высота растра
NOT_EXPORTED int32_t g_value {32}; // параметр яркости от эффекта

extern "C" EXPORTED void PLUG_CALL plugin_init(const struct context_t* context,
struct result_t* result) {
  // описание плагина
  result->full_name = "Brightness";
  result->author = "HPW-dev";
  result->description = "Add brightness";
  // проверка валидности данных
  if ( !check_params(context, result))
    return;
  // бинд параметров
  static_assert(sizeof(pal8_t) == sizeof(Pal8));
  g_dst = ptr2ptr<Pal8*>(context->dst);
  g_h = context->h;
  g_w = context->w;
  // создание строки в меню для управления параметром яркости
  context->registrate_param_i32(
    "value",
    "brightness value",
    &g_value,
    4, -180, 180
  );
} // plugin_init

extern "C" EXPORTED void PLUG_CALL plugin_apply(uint32_t state) {
  #pragma omp parallel for simd schedule(static, 4)
  cfor (i, g_w * g_h) {
    auto rgb = to_rgb24(g_dst[i]);
    rgb.r = std::clamp<int32_t>(rgb.r + g_value, 0, 255);
    rgb.g = std::clamp<int32_t>(rgb.g + g_value, 0, 255);
    rgb.b = std::clamp<int32_t>(rgb.b + g_value, 0, 255);
    g_dst[i] = desaturate_bt601(rgb.r, rgb.g, rgb.b);
  }
}

extern "C" EXPORTED void PLUG_CALL plugin_finalize(void) {}
