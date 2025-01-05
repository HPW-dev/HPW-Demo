#pragma once
#include <cstdint>
#include "util/math/vec.hpp"

class Image;

// отображает эффект искажения воздуха
class Heat_distort final {
  real max_duration {}; // начальная длительность эффекта
  real cur_duration {}; // текущая длительность эффекта

public:
  struct Flags {
    bool random_block_count: 1 {};
    bool random_radius: 1 {};
    bool random_block_size: 1 {};
    bool random_power: 1 {};
    bool infinity_duration: 1 {};
    bool decrease_radius: 1 {}; // уменьшать радиус эффекта с течением времени
    bool invert_decrease_radius: 1 {}; // увеличивать радиус со временем
    bool decrease_power: 1 {};
    bool decrease_block_size: 1 {};
    bool repeat: 1 {}; // перезапускать таймер по завершению
  }; // Flags

  Flags flags {};
  std::size_t radius {}; // в радиус отображения эффекта
  std::size_t block_size {}; // размер искажаемого блока
  real power {}; // как далеко блок смещается
  std::size_t block_count {}; // сколько искажаемых блоков рисовать

  Heat_distort() = default;
  ~Heat_distort() = default;

  void set_duration(real new_duration);
  inline cr<real> get_max_duration() const { return max_duration; }
  inline cr<real> get_cur_duration() const { return cur_duration; }

  void update(const Delta_time dt);
  void draw(Image& dst, const Veci offset);
  void restart();
}; // Heat_distort
