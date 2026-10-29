#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

class Menu;

// меню настроек управления
class Scene_input final: public Scene {
  Shared<Menu> menu {};
  
  void init_menu();

public:
  constx Str NAME = "input";

  Scene_input();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }
};
