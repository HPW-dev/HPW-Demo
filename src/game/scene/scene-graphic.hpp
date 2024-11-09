#pragma once
#include "scene.hpp"
#include "util/mem-types.hpp"

// меню настроек графики
class Scene_graphic final: public Scene {
  nocopy(Scene_graphic);
  struct Impl;
  Unique<Impl> _impl {};

public:
  Scene_graphic();
  ~Scene_graphic();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};

void set_high_quality();
