#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

class Menu;

/// меню отладки
class Scene_debug final: public Scene {
  Shared<Menu> menu {};

  void init_menu();

public:
  Scene_debug();
  void update(double dt) override;
  void draw(Image& dst) const override;
};
