#pragma once
#include "scene.hpp"
#include "util/mem-types.hpp"

class Menu;
  
// стартовое меню игры
class Scene_options final: public Scene {
  Shared<Menu> menu {};

  void init_menu();
  
public:
  Scene_options();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
