#include <cassert>
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "heat-distort.hpp"
#include "util/math/vec.hpp"
#include "util/math/random.hpp"
#include "util/math/mat.hpp"

void Heat_distort::set_duration(real new_duration) {
  assert(new_duration >= 0);
  max_duration = cur_duration = new_duration;
}

void Heat_distort::restart() {
  cur_duration = max_duration;
}

void Heat_distort::update(const Delta_time dt) {
  return_if (dt <= 0);
  return_if (dt >= 10);
  return_if(flags.infinity_duration);
  
  if (cur_duration <= 0 && flags.repeat)
    restart();
  cur_duration -= dt;
}

void Heat_distort::draw(Image& dst, const Vec offset) {
  return_if(cur_duration <= 0 && !flags.infinity_duration);
  assert(dst);

  auto time_scale = safe_div(cur_duration, max_duration);
  auto local_radius = radius;

  // изменить радиус в зависимости от времени
  if (flags.decrease_radius) {
    if (flags.invert_decrease_radius)
      local_radius = radius * (1.0 - time_scale);
    else
      local_radius = radius * time_scale;
  }
  // изменить силу эффекта в зависимости от времени
  auto local_power = flags.decrease_power
    ? power * time_scale
    : power;
  // изменить размер копируемого блока в зависимости от времени    
  auto pre_block_size = flags.decrease_block_size
    ? block_size * time_scale
    : block_size;
  // случайный размер блока
  auto local_block_count = flags.random_block_count
    ? rndr_fast(0, block_count)
    : block_count;
  // оптимизация пребуфера
  static Image cuted_block;
  cuted_block.init(0, 0);

  // в радиусе эффекта клонировать кусочки и сдвигать их немного от исходника
  cfor (block_idx, local_block_count) {
    // позиция для вырезания блока
    auto block_offset = rand_normalized_graphic();
    block_offset *= flags.random_radius
      ? rndr_fast(0, local_radius)
      : local_radius;
    // ищем блок для копирования
    auto local_block_size = flags.random_block_size
      ? rnd_fast(0, pre_block_size)
      : pre_block_size;
    Rect block;
    block.pos.x = block_offset.x + offset.x - local_block_size / 2;
    block.pos.y = block_offset.y + offset.y - local_block_size / 2;
    block.size.x = local_block_size;
    block.size.y = local_block_size;
    cuted_block = cut(dst, block, Image_get::MIRROR);
    // вставляем блок в новое место
    auto local_offset = rand_normalized_graphic();
    local_offset *= flags.random_power
      ? rndr_fast(0, local_power)
      : local_power;
    insert( dst, cuted_block,
      Vec{block.pos.x + local_offset.x, block.pos.y + local_offset.y} );
  } // for block_count
} // draw
