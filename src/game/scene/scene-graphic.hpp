#pragma once
#include "scene.hpp"
#include "util/mem-types.hpp"

class Menu_text_item;

// меню настроек графики
class Scene_graphic final: public Scene {
  nocopy(Scene_graphic);
  struct Impl;
  Unique<Impl> _impl {};

public:
  Scene_graphic();
  ~Scene_graphic();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};

// устанавлиевает выскоие настройки графики
void set_high_quality();
// пункт меню для выбора .dll/.so плагинов графики
Shared<Menu_text_item> get_shared_plugin_item();
