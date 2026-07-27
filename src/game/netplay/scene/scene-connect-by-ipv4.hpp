#pragma once
#include "util/mem-types.hpp"
#include "game/scene/scene.hpp"

// подключение к серверу по IPv4:Port
class Scene_connect_by_ipv4 final: public Scene {
  struct Impl;
  Unique<Impl> _impl {};

public:
  Scene_connect_by_ipv4();
  ~Scene_connect_by_ipv4();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return "connect by IPv4"; }
};
