#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

// окно смены палитры
class Scene_palette_select final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  constx Str NAME = "palette select";

  Scene_palette_select();
  ~Scene_palette_select();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }
};
