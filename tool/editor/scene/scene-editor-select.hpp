#pragma once
#include "scene.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"

// меню выбора редакторов
class Scene_editor_select: public Editor_scene_base {
  struct Impl;
  Unique<Impl> impl {};

public:
  Scene_editor_select();
  ~Scene_editor_select();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  void imgui_exec() override;
};
