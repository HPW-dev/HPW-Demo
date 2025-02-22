#pragma once
#include "util/mem-types.hpp"
#include "game/scene/scene.hpp"

// меню настроек игры
class Scene_options final: public Scene {
  struct Impl;
  Unique<Impl> _impl {};

public:
  Scene_options();
  ~Scene_options();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return "options"; }
};
