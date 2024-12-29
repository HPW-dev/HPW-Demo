#pragma once
#include "scene.hpp"
#include "util/mem-types.hpp"
#include "util/vector-types.hpp"
#include "util/math/num-types.hpp"

class Window;

// редактор анимаций
class Scene_anim_editor: public Editor_scene_base {
  Shared<Window> root {};

public:
  Scene_anim_editor();
  ~Scene_anim_editor() = default;
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  void post_draw(Image& dst) const;
  void imgui_exec() override;
};
