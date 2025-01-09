#pragma once
#include "util/mem-types.hpp"
#include "game/scene/scene.hpp"

// меню выбора для сетевой игры
class Scene_netplay_menu final: public Scene {
  struct Impl;
  Unique<Impl> _impl {};

public:
  Scene_netplay_menu();
  ~Scene_netplay_menu();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
