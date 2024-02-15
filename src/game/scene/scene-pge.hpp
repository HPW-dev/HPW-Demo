#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

/// окно смены плагина с эффектом
class Scene_pge final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  Scene_pge();
  ~Scene_pge();
  void update(double dt) override;
  void draw(Image& dst) const override;
};
