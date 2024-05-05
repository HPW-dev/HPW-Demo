#pragma once
#include "scene.hpp"
#include "util/mem-types.hpp"

class Menu;

// таблица рекордов
class Scene_record_table final: public Scene {
  Shared<Menu> menu {};

  void init_menu();

public:
  Scene_record_table();
  void update(double dt) override;
  void draw(Image& dst) const override;
};
