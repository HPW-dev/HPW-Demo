#pragma once
#include "level.hpp"
#include "util/math/num-types.hpp"

// уровень для теста нагрузки системы коллизий
class Level_collision_test final: public Level {
  nocopy(Level_collision_test);
  real brightness {};

  static void hitbox_test(); // тестирование функционала хитбоксов

public:
  Level_collision_test();
  ~Level_collision_test() = default;
  void update(const Vec vel, Delta_time dt);
  void draw(Image& dst) const;
  Str level_name() const override;
}; // Level_collision_test
