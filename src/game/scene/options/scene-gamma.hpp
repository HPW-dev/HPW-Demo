#pragma once
#include "game/scene/scene.hpp"


// настройки гаммы
class Scene_gamma final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  staticx Cstr NAME = "gamma";

  Scene_gamma();
  ~Scene_gamma();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }
};
