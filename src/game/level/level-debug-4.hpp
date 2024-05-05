#pragma once
#include "level.hpp"
#include "util/mem-types.hpp"

// Пустой уровень для тестов
class Level_debug_4 final: public Level {
  nocopy(Level_debug_4);
  struct Impl;
  Unique<Impl> impl {};
  
public:
  Level_debug_4();
  ~Level_debug_4();
  void update(const Vec vel, double dt) override;
  void draw(Image& dst) const override;
  Str level_name() const override;
}; // Level_debug_4
