#pragma once
#include <functional>
#include "scene.hpp"
#include "util/vector-types.hpp"

class Sprite;

// фон загрузки
class Scene_loading final: public Scene {
  std::function<void ()> scene_maker {};
  mutable bool drawed {false};
  bool used {false};
  int time_out {10};
  const Sprite* bg {}; // фон

public:
  Scene_loading(std::function<void ()>&& _scene_maker);
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return "loading"; }
};
