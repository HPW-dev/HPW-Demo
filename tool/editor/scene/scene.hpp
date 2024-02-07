#pragma once
#include "util/macro.hpp"
#include "game/scene/scene.hpp"

/// Сцены редактора с imGui графикой
class Editor_scene_base: public Scene {
  nocopy(Editor_scene_base);

public:
  Editor_scene_base() = default;
  ~Editor_scene_base() = default;
  virtual void imgui_exec() = 0;
}; // Editor_scene_base
