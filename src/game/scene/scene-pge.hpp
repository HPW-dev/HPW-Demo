#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

// окно смены плагина с эффектом
class Scene_pge final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  constx Str NAME = "PGE";

  Scene_pge();
  ~Scene_pge();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }
};
