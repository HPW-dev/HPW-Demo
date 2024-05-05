#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

// окно геймовера
class Scene_gameover final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  Scene_gameover();
  ~Scene_gameover();
  void update(double dt) override;
  void draw(Image& dst) const override;
};
