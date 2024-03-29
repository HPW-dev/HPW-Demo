#include <omp.h>
#include <ctime>
#include <algorithm>
#include "plugin/graphic-effect/hpw-plugin-effect.h"
#include "pge-util.hpp"
#include "util/macro.hpp"
#include "util/math/random.hpp"
#include "graphic/image/color.hpp"
#include "graphic/util/convert.hpp"

NOT_EXPORTED Pal8* g_dst {}; // ссыль на растр от игры
NOT_EXPORTED uint16_t g_w {}; // ширина растра
NOT_EXPORTED uint16_t g_h {}; // высота растра
NOT_EXPORTED int32_t g_mode {}; /// режим смешивания

extern "C" EXPORTED void PLUG_CALL plugin_init(const struct context_t* context,
struct result_t* result) {
  set_rnd_seed( time({}) );
  // описание плагина
  result->full_name = "Epilepsy";
  result->author = "HPW-dev";
  result->description = "Randomly pixel blending every frame";
  // проверка валидности данных
  if ( !check_params(context, result))
    return;
  // бинд параметров
  static_assert(sizeof(pal8_t) == sizeof(Pal8));
  g_dst = ptr2ptr<Pal8*>(context->dst);
  g_h = context->h;
  g_w = context->w;
  context->registrate_param_i32(
    "style",
    "pixel blending modes:\n"
    "  0 - xor;\n"
    "  1 - or;\n"
    "  2 - and.",
    &g_mode, 1, 0, 2
  );
} // plugin_init

extern "C" EXPORTED void PLUG_CALL plugin_apply(uint32_t state) {
  cauto rnd = rndu_fast() % 256u;

  switch (g_mode) {
    default:
    case 0: {
      #pragma omp parallel for simd schedule(static, 4)
      cfor (i, g_w * g_h)
        g_dst[i].val ^= rnd;
      break;
    }
    case 1: {
      #pragma omp parallel for simd schedule(static, 4)
      cfor (i, g_w * g_h)
        g_dst[i].val |= rnd;
      break;
    }
    case 2: {
      #pragma omp parallel for simd schedule(static, 4)
      cfor (i, g_w * g_h)
        g_dst[i].val &= rnd;
      break;
    }
  } // switch mode
} // plugin_apply

extern "C" EXPORTED void PLUG_CALL plugin_finalize(void) {}
