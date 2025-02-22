#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

// выбор реплея
class Scene_replay_select final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  Scene_replay_select();
  ~Scene_replay_select();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return "replay select"; }
};
