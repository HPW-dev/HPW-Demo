#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

// настройки гаммы
class Scene_gamma final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  constx Str NAME = "gamma";

  Scene_gamma();
  ~Scene_gamma();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }
};
