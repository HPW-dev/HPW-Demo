#pragma once
#include "scene.hpp"
#include "util/mem-types.hpp"

/// проверка чексум экзешника и данных игры
class Scene_validation final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  Scene_validation();
  ~Scene_validation();
  void update(double dt) override;
  void draw(Image& dst) const override;
};
