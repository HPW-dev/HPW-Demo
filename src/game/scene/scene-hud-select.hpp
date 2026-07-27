#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

// меню выбора HUD
class Scene_hud_select final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  constx Str NAME = "HUD select";

  Scene_hud_select();
  ~Scene_hud_select();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }
};
