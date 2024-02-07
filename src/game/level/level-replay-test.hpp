#pragma once
#ifdef DEBUG
#include "level.hpp"
#include "util/mem-types.hpp"

class Phys;

class Level_replay_test final: public Level {
  nocopy(Level_replay_test);
  struct Impl;
  Unique<Impl> impl {};

public:
  Level_replay_test();
  ~Level_replay_test();
  void update(const Vec vel, double dt);
  void draw(Image& dst) const;
}; // Level_replay_test

#endif // DEBUG
