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
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return "game over"; }
};
