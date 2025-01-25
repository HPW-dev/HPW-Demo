#pragma once
#include "util/mem-types.hpp"
#include "game/scene/scene.hpp"
#include "util/str.hpp"
#include "util/net/net.hpp"

// Окно клиента
class Client final: public Scene {
  struct Impl;
  Unique<Impl> _impl {};

public:
  explicit Client(cr<Str> ip_v4, const net::Port port);
  ~Client();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
