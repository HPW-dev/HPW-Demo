#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

class Menu;

// окно паузы игры
class Scene_game_pause final: public Scene {
  Shared<Menu> menu {};

  void init_menu();

public:
  Scene_game_pause();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return "game pause"; }
};
