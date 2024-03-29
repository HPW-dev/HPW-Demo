#include <omp.h>
#include <array>
#include "plugin/graphic-effect/hpw-plugin-effect.h"
#include "pge-util.hpp"
#include "util/macro.hpp"
#include "graphic/image/image.hpp"
#include "graphic/image/color.hpp"
#include "graphic/util/convert.hpp"

NOT_EXPORTED Pal8* g_dst {}; // ссыль на растр от игры
NOT_EXPORTED uint16_t g_w {}; // ширина растра
NOT_EXPORTED uint16_t g_h {}; // высота растра
NOT_EXPORTED int32_t g_mode {1}; // режим мерцания (0 - 2 кадра, 1 - много)
NOT_EXPORTED void init_tables();
NOT_EXPORTED inline Pal8 get_from_table_0(const Pal8 src, uint32_t state);
NOT_EXPORTED inline Pal8 get_from_table_1(const Pal8 src, uint32_t state);

extern "C" EXPORTED void PLUG_CALL plugin_init(const struct context_t* context,
struct result_t* result) {
  // описание плагина
  result->full_name = "Blink-dithering";
  result->author = "HPW-dev";
  result->description = "Color-innertion dithering withm small palette";
  // проверка валидности данных
  if ( !check_params(context, result))
    return;
  // бинд параметров
  static_assert(sizeof(pal8_t) == sizeof(Pal8));
  g_dst = ptr2ptr<Pal8*>(context->dst);
  g_h = context->h;
  g_w = context->w;
  context->registrate_param_i32(
    "mode",
    "modes:\n"
    "  0 - black, white, red;\n"
    "  1 - black, white, red with more blink-frames.",
    &g_mode, 1, 0, 1
  );
  init_tables();
} // plugin_init

extern "C" EXPORTED void PLUG_CALL plugin_apply(uint32_t state) {
  if (g_mode == 0) {
    #pragma omp parallel for simd schedule(static, 4)
    cfor (i, g_w * g_h)
      g_dst[i] = get_from_table_0(g_dst[i], state);
  } else {
    #pragma omp parallel for simd schedule(static, 4)
    cfor (i, g_w * g_h)
      g_dst[i] = get_from_table_1(g_dst[i], state);
  }
}

extern "C" EXPORTED void PLUG_CALL plugin_finalize(void) {}

NOT_EXPORTED constexpr const std::size_t line_sz = 256;
/// black, white, red [black .. blink .. white]
NOT_EXPORTED std::array<Pal8::value_t, line_sz * 2> table_0 {};
/// black, white, red [black .. blink .. white] окно в 5 кадров мерцания
NOT_EXPORTED std::array<Pal8::value_t, line_sz * 6> table_1 {};

NOT_EXPORTED void init_tables() {
  // table_0
  cfor (state, 2) {
    cfor (i, line_sz) {
      const Pal8 color(i);
      cauto is_red = color.is_red();
      cauto cr = color.to_real();

      if (cr <= 1.0 / 3.0) {
        table_0.at(i + line_sz * state) = 0;
      } else if (cr <= 2.0 / 3.0) {
        table_0.at(i + line_sz * state) = Pal8::from_real(1 * state, is_red);
      } else {
        table_0.at(i + line_sz * state) = Pal8::from_real(1, is_red);
      }
    }
  } // table_0

  // table_1
  cauto is_blink = [](const real val, const uint32_t state)->int {
    switch (state) {
      default:
      case 5: return 1;
      case 4: return (val >= 0.8) ? 1 : 0;
      case 3: return (val >= 0.6) ? 1 : 0;
      case 2: return (val >= 0.4) ? 1 : 0;
      case 1: return (val >= 0.2) ? 1 : 0;
      case 0: return 0;
    }
    return 0;
  };

  cfor (state, 6) {
    cfor (i, line_sz) {
      const Pal8 color(i);
      cauto is_red = color.is_red();
      cauto cr = color.to_real();
      table_1.at(i + line_sz * state) = Pal8::from_real(1 * is_blink(cr, state), is_red);
    }
  } // table_1
}

NOT_EXPORTED inline Pal8 get_from_table_0(const Pal8 src, uint32_t state) {
  return table_0[src.val + line_sz * (state & 1u)];
}

NOT_EXPORTED inline Pal8 get_from_table_1(const Pal8 src, uint32_t state) {
  return table_1[src.val + line_sz * (state % 5u)];
}
