#pragma once
#include "level.hpp"
#include "util/vector-types.hpp"
#include "util/mem-types.hpp"

class Phys;

// Уровень для теста phys
class Level_debug_1 final: public Level {
  nocopy(Level_debug_1);
  Vector<Shared<Phys>> objs {};

  void draw_bg(Image& dst) const;
  void draw_obj(CN<Phys> obj, Image& dst) const;
  void bound_check(Phys& obj) const;

public:
  Level_debug_1();
  ~Level_debug_1() = default;
  void update(const Vec vel, Delta_time dt);
  void draw(Image& dst) const;
}; // Level_debug_1
