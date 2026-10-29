#pragma once
#include "game/scene/scene.hpp"


class Menu;

// таблица рекордов
class Scene_record_table final: public Scene {
  Shared<Menu> menu {};

  void init_menu();

public:
  staticx Cstr NAME = "record table";

  Scene_record_table();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }
};
