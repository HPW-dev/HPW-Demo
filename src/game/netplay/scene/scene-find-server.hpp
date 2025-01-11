#pragma once
#include "util/mem-types.hpp"
#include "game/scene/scene.hpp"

// меню поиска сервера
class Scene_find_server final: public Scene {
  struct Impl;
  Unique<Impl> _impl {};

public:
  Scene_find_server();
  ~Scene_find_server();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
