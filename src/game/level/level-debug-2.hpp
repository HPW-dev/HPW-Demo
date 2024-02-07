#pragma once
#include "level.hpp"
#include "util/math/vec.hpp"

/// Уровень для теста статтеринга
class Level_debug_2 final: public Level {
  nocopy(Level_debug_2);
  real brightness {};

  Vec pos {};
  Vec pos_old {};
  Vec pos_future {};
  Vec pos_future_old {};
  mutable Vec interpolated_old {};

public:
  Level_debug_2();
  ~Level_debug_2() = default;
  void update(const Vec vel, double dt);
  void draw(Image& dst) const;
}; // Level_debug_2
