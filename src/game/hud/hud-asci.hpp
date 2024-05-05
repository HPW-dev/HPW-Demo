#pragma once
#include "hud.hpp"
#include "util/mem-types.hpp"

// Буквенный интерфейс игрока
class Hud_asci: public Hud {
  struct Impl;
  Unique<Impl> impl {};

public:
  Hud_asci();
  ~Hud_asci();
  void draw(Image& dst) const override;
  void update(double dt) override;
}; // Hud_asci

