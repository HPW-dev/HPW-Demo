#pragma once
#include "hud.hpp"
#include "util/mem-types.hpp"

// интерфейс игрока с римскими цифрами
class Hud_roman: public Hud {
  struct Impl;
  Unique<Impl> impl {};

public:
  Hud_roman();
  ~Hud_roman();
  void draw(Image& dst) const override;
  void update(const Delta_time dt) override;
};
