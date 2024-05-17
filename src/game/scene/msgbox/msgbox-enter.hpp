#pragma once
#include <functional>
#include "util/mem-types.hpp"
#include "util/unicode.hpp"
#include "game/scene/scene.hpp"

// Окно с текстом. Для выхода нажать Enable/Enter
class Msgbox_enter final: public Scene {
  struct Impl;
  Unique<Impl> impl {};
  using Action = std::function<void ()>;

public:
  explicit Msgbox_enter(CN<utf32> msg, CN<utf32> title={}, CN<Action> action={});
  ~Msgbox_enter();
  void update(double dt) override;
  void draw(Image& dst) const override;
};
