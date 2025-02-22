#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

// список для выбора встроенных эффектов
class Scene_epge_list final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  Scene_epge_list();
  ~Scene_epge_list();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return "EPGE list"; }
};
