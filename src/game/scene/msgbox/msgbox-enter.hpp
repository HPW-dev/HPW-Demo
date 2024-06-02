#pragma once
#include <functional>
#include "util/mem-types.hpp"
#include "util/unicode.hpp"
#include "game/scene/scene.hpp"

// Окно с текстом. Для выхода нажать Enable/Enter
class Scene_msgbox_enter final: public Scene {
  struct Impl;
  Unique<Impl> impl {};
  using Action = std::function<void ()>;

public:
  explicit Scene_msgbox_enter(CN<utf32> msg, CN<utf32> title={}, CN<Action> action={});
  ~Scene_msgbox_enter();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
