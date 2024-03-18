#include <omp.h>
#include <cassert>
#include <ctime>
#include "plugin/graphic-effect/hpw-plugin-effect.h"
#include "pge-util.hpp"
#include "util/macro.hpp"
#include "util/math/random.hpp"
#include "util/vector-types.hpp"
#include "graphic/image/color.hpp"
#include "graphic/util/convert.hpp"

NOT_EXPORTED Pal8* g_dst {}; // ссыль на растр от игры
NOT_EXPORTED uint16_t g_w {}; // ширина растра
NOT_EXPORTED uint16_t g_h {}; // высота растра
NOT_EXPORTED int32_t g_length {7}; /// сколько кадров перемешивать
NOT_EXPORTED int32_t g_old_length {}; /// чтобы определять изменение параметра
using Buffer = Vector<Pal8>;
NOT_EXPORTED Vector<Buffer> g_buffers {};
NOT_EXPORTED std::size_t g_idx {0};

NOT_EXPORTED void resize_buffers(const int32_t sz) {
  assert(sz > 1);
  assert(sz < 999'999);
  g_buffers.resize(sz);
  for (nauto buffer: g_buffers)
    buffer.resize(g_w * g_h);
}

extern "C" EXPORTED void PLUG_CALL plugin_init(const struct context_t* context,
struct result_t* result) {
  set_rnd_seed( time({}) );
  // описание плагина
  result->full_name = "Random frame";
  result->description = "Randomize frames";
  // проверка валидности данных
  if ( !check_params(context, result))
    return;
  // бинд параметров
  static_assert(sizeof(pal8_t) == sizeof(Pal8));
  g_dst = ptr2ptr<Pal8*>(context->dst);
  g_h = context->h;
  g_w = context->w;
  context->registrate_param_i32(
    "buffer length",
    "More buffer length means chaos",
    &g_length, 1, 2, 100
  );
  g_old_length = g_length;
  resize_buffers(g_length);
  // начальная заливка всех буфферов одним и тем же кадром
  for (nauto buffer: g_buffers)
    memcpy( ptr2ptr<Pal8*>(buffer.data()),
      g_dst, buffer.size() * sizeof(Pal8) );
} // plugin_init

extern "C" EXPORTED void PLUG_CALL plugin_apply(uint32_t state) {
  // если поменялся g_length, то создать новые буфферы
  if (g_length != g_old_length)
    resize_buffers(g_length);
  g_old_length = g_length;
  // сейв текущего буфера 
  assert(g_length != 0);
  cauto idx = state % g_length;
  memcpy( ptr2ptr<Pal8*>(g_buffers.at(idx).data()),
    g_dst, g_buffers.at(idx).size() * sizeof(Pal8) );
  // отрисовка случайного буффера
  cauto rnd_idx = rndu_fast(g_length - 1);
  memcpy( g_dst, cptr2ptr<CP<Pal8>>(g_buffers.at(rnd_idx).data()),
    g_buffers.at(rnd_idx).size() * sizeof(Pal8) );
} // plugin_apply

extern "C" EXPORTED void PLUG_CALL plugin_finalize(void) {
  g_buffers.clear();
}
