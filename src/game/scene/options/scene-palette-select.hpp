#pragma once
#include "game/scene/scene.hpp"

// окно смены палитры
class Scene_palette_select final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  staticx Cstr NAME = "palette select";

  Scene_palette_select();
  ~Scene_palette_select();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }
};
