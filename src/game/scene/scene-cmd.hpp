#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

// окно ввода команд
class Scene_cmd final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  Scene_cmd();
  ~Scene_cmd();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
