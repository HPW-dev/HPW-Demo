#pragma once
#include "scene.hpp"
#include "util/mem-types.hpp"
#include "util/vector-types.hpp"
#include "util/math/num-types.hpp"

class Window;

class Editor_scene: public Editor_scene_base {
  Shared<Window> root {};

public:
  Editor_scene();
  ~Editor_scene() = default;
  void update(double dt) override;
  void draw(Image& dst) const override;
  void imgui_exec() override;
}; // Editor_scene
