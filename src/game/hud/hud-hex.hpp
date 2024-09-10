#pragma once
#include "hud.hpp"
#include "util/mem-types.hpp"

// Буквенный интерфейс игрока
class Hud_hex: public Hud {
  struct Impl;
  Unique<Impl> impl {};

public:
  Hud_hex();
  ~Hud_hex();
  void draw(Image& dst) const override;
  void update(const Delta_time dt) override;
};
