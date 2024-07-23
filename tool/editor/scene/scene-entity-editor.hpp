#pragma once
#include "scene.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"

// редактор игровых объектов
class Scene_entity_editor: public Editor_scene_base {
  struct Impl;
  Unique<Impl> impl {};

public:
  Scene_entity_editor();
  ~Scene_entity_editor();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  void imgui_exec() override;
  void exit();
  void pause();
  void save();
  void reload();
  void add_emitter(); // создаёт окошко для настроек спавна объектов
};
