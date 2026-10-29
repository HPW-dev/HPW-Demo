#pragma once
#include "game/scene/scene.hpp"
#include "util/unicode.hpp"


// окно ввода никнейма
class Scene_nickname final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  staticx Cstr NAME = "Scene_nickname";

  Scene_nickname();
  ~Scene_nickname();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }
};
