#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

// настройки гаммы
class Scene_game_options final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  Scene_game_options();
  ~Scene_game_options();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
