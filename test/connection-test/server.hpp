#pragma once
#include "util/mem-types.hpp"
#include "game/scene/scene.hpp"

// Окно сервера
class Server final: public Scene {
  struct Impl;
  Unique<Impl> _impl {};

public:
  Server();
  ~Server();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
