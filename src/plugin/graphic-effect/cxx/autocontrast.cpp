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

extern "C" EXPORTED void PLUG_CALL plugin_init(const struct context_t* context,
struct result_t* result) {
  // описание плагина
  result->full_name = "AutoContrast";
  result->author = "HPW-dev";
  result->description = "Auto-correction of color contrast";
  // проверка валидности данных
  return_if( !check_params(context, result));
  // бинд параметров
  static_assert(sizeof(pal8_t) == sizeof(Pal8));
  g_dst = ptr2ptr<Pal8*>(context->dst);
  g_h = context->h;
  g_w = context->w;
}

extern "C" EXPORTED void PLUG_CALL plugin_apply(uint32_t state) {
  // узнать максимальный и минимальный по яркости цвет на картинке
  cauto SZ = g_w * g_h;
  real min_luma {g_dst[0].to_real()};
  real max_luma {g_dst[0].to_real()};
  real avr_luma {}; // средний оттенок на всём кадре
  cfor (i, SZ) {
    cauto COLOR = g_dst[i].to_real();
    min_luma = std::min(min_luma, COLOR);
    max_luma = std::max(max_luma, COLOR);
    avr_luma += COLOR;
  }
  avr_luma /= SZ;

  // растянуть цвета под самый чёрный и светлый оттенок
  cauto DIFF = max_luma - min_luma;
  cauto MUL = (DIFF == 0 ? 0 : 1.f / DIFF);
  cauto contrast = (1.f - avr_luma) * 2.f;
  #pragma omp parallel for simd
  cfor (i, SZ) {
    cauto RED = g_dst[i].is_red();
    auto luma = g_dst[i].to_real();
    // растягивание градиента
    luma = (luma - min_luma) * MUL;
    // контраст
    luma = (luma - 0.5) * contrast + 0.5;
    g_dst[i] = Pal8::from_real(luma, RED);
  }
}

extern "C" EXPORTED void PLUG_CALL plugin_finalize(void) {}
