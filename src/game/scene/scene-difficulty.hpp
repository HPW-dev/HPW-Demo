#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

class Menu;

// меню выбора сложности
class Scene_difficulty final: public Scene {
  Shared<Menu> menu {};

  void init_menu();

public:
  constx Str NAME = "difficulty";

  Scene_difficulty();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }
};
