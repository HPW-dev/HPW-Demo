#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

// настройки гаммы
class Scene_gamma final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  Scene_gamma();
  ~Scene_gamma();
  void update(double dt) override;
  void draw(Image& dst) const override;
};
