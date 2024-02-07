#pragma once
#include "level.hpp"

class Level_1 final: public Level {
  nocopy(Level_1);

public:
  Level_1() = default;
  ~Level_1() = default;
  void update(const Vec vel, double dt);
  void draw(Image& dst) const;
}; // Level_1
