#pragma once
#include "util/mem-types.hpp"
#include "util/str.hpp"
#include "util/net/net.hpp"
#include "game/scene/scene.hpp"

// Окно сервера
class Server final: public Scene {
  struct Impl;
  Unique<Impl> _impl {};

public:
  explicit Server(cr<Str> ip_v4, const net::Port port);
  ~Server();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
