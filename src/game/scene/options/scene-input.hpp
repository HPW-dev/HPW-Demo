#pragma once
#include "game/scene/scene.hpp"


class Menu;

// меню настроек управления
class Scene_input final: public Scene {
  Shared<Menu> menu {};
  
  void init_menu();

public:
  staticx Cstr NAME = "input";

  Scene_input();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }
};
