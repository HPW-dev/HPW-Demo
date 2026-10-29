#pragma once
#include "game/scene/scene.hpp"


// выбор встроенных эффектов
class Scene_epge final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  staticx Cstr NAME = "EPGE";

  Scene_epge();
  ~Scene_epge();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }
};
