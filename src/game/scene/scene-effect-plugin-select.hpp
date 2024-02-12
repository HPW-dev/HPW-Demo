#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

/// окно смены плагина с эффектом
class Scene_effect_plugin_select final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  Scene_effect_plugin_select();
  ~Scene_effect_plugin_select();
  void update(double dt) override;
  void draw(Image& dst) const override;
};
