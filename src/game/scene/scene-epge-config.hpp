#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

namespace epge { class Base; }

// меню настройки конкретного эффекта
class Scene_epge_config final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Scene_epge_config(epge::Base* epge);
  ~Scene_epge_config();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return "EPGE config"; }
};
