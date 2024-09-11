#include <omp.h>
#include <cassert>
#include <cmath>
#include "plugin/graphic-effect/hpw-plugin-effect.h"
#include "pge-util.hpp"
#include "util/macro.hpp"
#include "graphic/image/color.hpp"

NOT_EXPORTED Pal8* g_dst {}; // ссыль на растр от игры
NOT_EXPORTED uint16_t g_w {}; // ширина растра
NOT_EXPORTED uint16_t g_h {}; // высота растра
NOT_EXPORTED int32_t g_colors {4}; // до скольки цветов урезать палитру

extern "C" EXPORTED void PLUG_CALL plugin_init(const struct context_t* context,
struct result_t* result) {
  // описание плагина
  result->full_name = "Posterize";
  result->author = "HPW-dev";
  result->description = "Color posterization effect";
  // проверка валидности данных
  return_if( !check_params(context, result));
  // бинд параметров
  static_assert(sizeof(pal8_t) == sizeof(Pal8));
  g_dst = ptr2ptr<Pal8*>(context->dst);
  g_h = context->h;
  g_w = context->w;
  context->registrate_param_i32(
    "colors",
    "from 2 to 16",
    &g_colors, 1, 2, 16
  );
}

extern "C" EXPORTED void PLUG_CALL plugin_apply(uint32_t state) {
  assert(g_colors > 1);
  assert(g_dst);
  const real MUL = 255.f / (g_colors-1);
  const real MUL2 = 255.f / g_colors;

  #pragma omp parallel for simd
  cfor (i, g_w * g_h) {
    rauto color = g_dst[i];
    cauto is_red = color.is_red();
    int c255 = color.to_real() * 255.f;
    c255 /= MUL2;
    color = Pal8::from_real(std::round(c255 * MUL) / 255.f, is_red);
  }
}

extern "C" EXPORTED void PLUG_CALL plugin_finalize(void) {}
