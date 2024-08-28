#pragma once
#include <functional>
#include "util/mem-types.hpp"
#include "util/unicode.hpp"
#include "game/scene/scene.hpp"

/* Окно с текстом. Для выхода нажать Enable/Enter.
Безопаснее создавать окно об ошибках через таск */
class Scene_msgbox_enter final: public Scene {
  struct Impl;
  Unique<Impl> impl {};
  using Action = std::function<void ()>;

public:
  explicit Scene_msgbox_enter(cr<utf32> msg, cr<utf32> title={}, cr<Action> action={});
  ~Scene_msgbox_enter();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
