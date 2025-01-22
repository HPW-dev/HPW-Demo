#pragma once
#include "util/mem-types.hpp"
#include "game/scene/scene.hpp"

// Окно клиента
class Client final: public Scene {
  struct Impl;
  Unique<Impl> _impl {};

public:
  Client();
  ~Client();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
