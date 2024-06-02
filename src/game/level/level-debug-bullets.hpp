#pragma once
#include "level.hpp"
#include "util/mem-types.hpp"

// Уровень для теста проскакивания пуль
class Level_debug_bullets final: public Level {
  nocopy(Level_debug_bullets);
  struct Impl;
  Unique<Impl> impl {};

public:
  Level_debug_bullets();
  ~Level_debug_bullets();
  void update(const Vec vel, Delta_time dt) override;
  void draw(Image& dst) const override;
}; // Level_debug_bullets
