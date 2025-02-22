#pragma once
#include "util/mem-types.hpp"
#include "game/scene/scene.hpp"

// выбор режима сервер/клиент
class Server_or_client final: public Scene {
  struct Impl;
  Unique<Impl> _impl {};

public:
  Server_or_client();
  ~Server_or_client();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return "server or client"; }
};
