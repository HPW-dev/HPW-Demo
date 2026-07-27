#pragma once
#include "scene.hpp"

// инфо о разрабах и т.п.
class Scene_info final: public Scene {
public:
  constx Str NAME = "info";

  Scene_info();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }
};
