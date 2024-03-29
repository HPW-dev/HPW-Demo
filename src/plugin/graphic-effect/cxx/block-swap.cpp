#include <omp.h>
#include <cassert>
#include <ctime>
#include <cstring>
#include "plugin/graphic-effect/hpw-plugin-effect.h"
#include "pge-util.hpp"
#include "util/macro.hpp"
#include "util/math/random.hpp"
#include "util/math/rect.hpp"
#include "util/vector-types.hpp"
#include "graphic/image/color.hpp"
#include "graphic/util/convert.hpp"

NOT_EXPORTED Pal8* g_dst {}; // ссыль на растр от игры
NOT_EXPORTED uint16_t g_w {}; // ширина растра
NOT_EXPORTED uint16_t g_h {}; // высота растра
NOT_EXPORTED int32_t g_block_sz {8}; /// размер блока
NOT_EXPORTED int32_t g_enable_block_swap {1}; /// менять ли блоки местами
NOT_EXPORTED int32_t g_enable_block_rotate {1}; /// вращать блоки

struct Block {
  enum class Rotation {
    deg0,
    deg90,
    deg180,
    deg270,
  };
  Rotation rotation {};
  Rect rect {};
};

NOT_EXPORTED Vector<Block> blocks {}; /// block map

void checko_opts() {
  return_if(g_enable_block_swap && g_enable_block_rotate);
  // если все опции выключены, то включить хотя бы что-то
  if (!g_enable_block_swap && !g_enable_block_rotate)
    g_enable_block_swap = true;
}

extern "C" EXPORTED void PLUG_CALL plugin_init(const struct context_t* context,
struct result_t* result) {
  set_rnd_seed( time({}) );
  // описание плагина
  result->full_name = "Block swap";
  result->description = "Random swap and rotate for blocks of image";
  // проверка валидности данных
  if ( !check_params(context, result))
    return;
  // бинд параметров
  static_assert(sizeof(pal8_t) == sizeof(Pal8));
  g_dst = ptr2ptr<Pal8*>(context->dst);
  g_h = context->h;
  g_w = context->w;
  context->registrate_param_i32(
    "Enable rotation",
    "Enable random rotation of image block",
    &g_enable_block_rotate, 1, 0, 1
  );
  context->registrate_param_i32(
    "Enable swap",
    "Enable random swap of image block",
    &g_enable_block_swap, 1, 0, 1
  );
} // plugin_init

extern "C" EXPORTED void PLUG_CALL plugin_apply(uint32_t state) {
  checko_opts();
} // plugin_apply

extern "C" EXPORTED void PLUG_CALL plugin_finalize(void) {}
