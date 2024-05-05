#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

class Menu;

// меню выбора сложности
class Scene_difficulty final: public Scene {
  Shared<Menu> menu {};

  void init_menu();

public:
  Scene_difficulty();
  void update(double dt) override;
  void draw(Image& dst) const override;
};
