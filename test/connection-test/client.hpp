#pragma once
#include "netcode.hpp"
#include "game/scene/scene.hpp"
#include "util/mem-types.hpp"
#include "util/net/net.hpp"

// Окно клиента
class Client final: public Scene {
  struct Impl;
  Unique<Impl> _impl {};

public:
  explicit Client(cr<Connection_ctx> ctx);
  ~Client();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
