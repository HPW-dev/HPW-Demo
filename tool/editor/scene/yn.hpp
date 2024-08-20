#pragma once
#include <functional>
#include "scene.hpp"
#include "util/unicode.hpp"

// окно подтверждения
class Yes_no_scene: public Editor_scene_base {
  nocopy(Yes_no_scene);
  utf8 msg_ {};
  std::function<void ()> callback {};
  void enable();

public:
  Yes_no_scene(cr<utf8> msg, cr<decltype(callback)> new_callback);
  void update(const Delta_time dt) override;
  void imgui_exec() override;
};
