#pragma once
#include "util/mem-types.hpp"
#include "util/unicode.hpp"
#include "scene.hpp"

// окно ввода никнейма
class Scene_nickname final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  Scene_nickname();
  ~Scene_nickname();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return "nickname"; }
};
