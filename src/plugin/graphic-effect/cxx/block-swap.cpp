#ifndef __clang__
#include <omp.h>
#endif

#include <iostream>
#include <random>
#include <algorithm>
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
NOT_EXPORTED Vector<Pal8> g_buffer {};
NOT_EXPORTED uint16_t g_w {}; // ширина растра
NOT_EXPORTED uint16_t g_h {}; // высота растра
NOT_EXPORTED int32_t g_block_sz {8}; // размер блока
NOT_EXPORTED int32_t g_block_sz_old {8};
NOT_EXPORTED int32_t g_enable_block_swap {1}; // менять ли блоки местами
NOT_EXPORTED int32_t g_enable_block_rotate {1}; // вращать блоки
NOT_EXPORTED std::minstd_rand g_rnd_generator;

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

NOT_EXPORTED bool g_need_update {true};
NOT_EXPORTED std::size_t g_block_y {};
NOT_EXPORTED std::size_t g_block_x {};
NOT_EXPORTED Vector<Block> blocks {}; // block map

inline void check_opts() {
  if (g_block_sz != g_block_sz_old) {
    g_block_sz_old = g_block_sz;
    g_need_update = true;
  }

  // если все опции выключены, то включить хотя бы что-то
  if (!g_enable_block_swap && !g_enable_block_rotate)
    g_enable_block_swap = true;
}

extern "C" EXPORTED void PLUG_CALL plugin_init(const struct context_t* context,
struct result_t* result) {
  set_rnd_seed( time({}) );
  g_rnd_generator.seed( time({}) );
  // описание плагина
  result->full_name = "Block swap";
  result->author = "HPW-dev";
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
    "Block size",
    "Size of image block",
    &g_block_sz, 8, 8, 128
  );
  g_block_sz_old = g_block_sz;
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
  g_buffer.resize(g_h * g_w);
} // plugin_init

inline Block::Rotation get_rnd_rotation() {
  switch (rndu_fast(4)) {
    default:
    case 0: return Block::Rotation::deg0; break;
    case 1: return Block::Rotation::deg90; break;
    case 2: return Block::Rotation::deg180; break;
    case 3: return Block::Rotation::deg270; break;
  }
  return {};
}

inline void update_block_map() {
  return_if (!g_need_update);
  g_need_update = false;

  assert(g_block_sz > 0 && g_block_sz < 1000);
  g_block_y = g_h / g_block_sz;
  g_block_x = g_w / g_block_sz;
  assert(g_block_x > 0);
  assert(g_block_y > 0);
  blocks.resize(g_block_x * g_block_y);

  cfor (y, g_block_y)
  cfor (x, g_block_x) {
    nauto block = blocks.at(y * g_block_x + x);
    block.rect.pos = Vec(x * g_block_sz, y * g_block_sz);
    block.rect.size = Vec(g_block_sz, g_block_sz);
    block.rotation = get_rnd_rotation();
  }
  std::shuffle(blocks.begin(), blocks.end(), g_rnd_generator);
}

inline void draw_block(const Block block, const Vec pos) {
  // TODO исправить
  const int st_y = block.rect.pos.y;
  const int st_x = block.rect.pos.x;
  const int ed_y = st_y + block.rect.size.y;
  const int ed_x = st_x + block.rect.size.x;
  for (int y = st_y; y < ed_y; ++y)
  for (int x = st_x; x < ed_x; ++x) {
    cauto src = get_pixel_safe(g_buffer.data(), x, y, g_w, g_h);
    set_pixel_safe(g_dst, pos.x + x, pos.y + y, g_w, g_h, src);
  }
}

extern "C" EXPORTED void PLUG_CALL plugin_apply(uint32_t state) {
  check_opts();
  update_block_map();
  assert( !g_buffer.empty() && g_buffer.size() == g_w * g_h);
  // buffer <- dst
  memcpy(ptr2ptr<void*>(g_buffer.data()), cptr2ptr<CP<void>>(g_dst),
    sizeof(Pal8) * g_w * g_h);

  // TODO исправить
  assert( !blocks.empty());
  cfor (y, g_block_y)
  cfor (x, g_block_x) {
    cnauto block = blocks.at(y * g_block_x + x);
    draw_block(block, Vec(x * g_block_sz, y * g_block_sz));
  }
} // plugin_apply

extern "C" EXPORTED void PLUG_CALL plugin_finalize(void) {
  g_buffer.clear();
}
