#pragma once
#include "util/mem-types.hpp"
#include "game/scene/scene.hpp"

// создание сервера
class Scene_create_server final: public Scene {
  struct Impl;
  Unique<Impl> _impl {};

public:
  Scene_create_server();
  ~Scene_create_server();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return "create server"; }
};
