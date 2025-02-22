#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

// выбор встроенных эффектов
class Scene_epge final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  Scene_epge();
  ~Scene_epge();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return "EPGE"; }
};
