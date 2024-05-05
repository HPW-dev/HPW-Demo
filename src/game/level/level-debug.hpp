#pragma once
#include "level.hpp"
#include "util/math/num-types.hpp"

// уровень для теста нагрузки системы коллизий
class Level_debug final: public Level {
  nocopy(Level_debug);
  real brightness {};

  static void hitbox_test(); // тестирование функционала хитбоксов

public:
  Level_debug();
  ~Level_debug() = default;
  void update(const Vec vel, double dt);
  void draw(Image& dst) const;
}; // Level_debug
