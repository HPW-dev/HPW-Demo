#pragma once
#include "game/scene/scene.hpp"

// окно ввода команд
class Scene_cmd final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  staticx Cstr NAME = "CMD";

  Scene_cmd();
  ~Scene_cmd();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }
};
