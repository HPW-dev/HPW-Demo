#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

// окно смены языка
class Scene_locale_select final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  Scene_locale_select();
  ~Scene_locale_select();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
