#pragma once
#include "netcode.hpp"
#include "util/mem-types.hpp"
#include "util/net/net.hpp"
#include "game/scene/scene.hpp"

// Окно сервера
class Server final: public Scene {
  struct Impl;
  Unique<Impl> _impl {};

public:
  explicit Server(const Connection_ctx ctx);
  ~Server();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
