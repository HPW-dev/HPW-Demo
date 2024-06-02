#pragma once
#include "util/mem-types.hpp"
#include "game/scene/scene.hpp"

class Cutscene_1 final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  Cutscene_1();
  ~Cutscene_1();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
}; // Cutscene_1
