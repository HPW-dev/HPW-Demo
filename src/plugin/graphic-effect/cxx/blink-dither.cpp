#include <omp.h>
#include "plugin/graphic-effect/hpw-plugin-effect.h"
#include <array>
#include "pge-util.hpp"
#include "util/macro.hpp"
#include "graphic/image/image.hpp"
#include "graphic/image/color.hpp"
#include "graphic/util/convert.hpp"

Pal8* g_dst {}; // ссыль на растр от игры
uint16_t g_w {}; // ширина растра
uint16_t g_h {}; // высота растра
int32_t g_mode {1}; // режим мерцания (0 - 2 кадра, 1 - много)
void init_tables();
inline Pal8 get_from_table_0(const Pal8 src, uint32_t state);
inline Pal8 get_from_table_1(const Pal8 src, uint32_t state);

extern "C" void plugin_init(const struct context_t* context, struct result_t* result) {
  // описание плагина
  result->full_name = "Blink-dithering";
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

extern "C" void plugin_apply(uint32_t state) {
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

extern "C" void plugin_finalize(void) {}

constexpr const std::size_t line_sz = 256;
/// black, white, red [black .. blink .. white]
std::array<Pal8::value_t, line_sz * 2> table_0 {};

void init_tables() {
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
  // TODO
}

inline Pal8 get_from_table_0(const Pal8 src, uint32_t state) {
  return table_0[src.val + line_sz * (state & 1u)];
}

inline Pal8 get_from_table_1(const Pal8 src, uint32_t state) {
  return table_0[src.val + line_sz * (state & 1u)]; // TODO
}
