#pragma once
#include "level.hpp"
#include "util/mem-types.hpp"

// Уровень для теста игрока
class Level_debug_3 final: public Level {
  nocopy(Level_debug_3);

  struct Impl;
  Unique<Impl> impl {};

  void draw_bg(Image& dst) const;
  void make_player();
  void make_dummy();
  void spawn_bullets(double dt);
  void save_dbg_info();

public:
  Level_debug_3();
  ~Level_debug_3();
  void update(const Vec vel, double dt) override;
  void draw(Image& dst) const override;
}; // Level_debug_3
