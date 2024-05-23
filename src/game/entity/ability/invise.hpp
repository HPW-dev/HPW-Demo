#pragma once
#include "ability.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"

// невидимость игрока, пуль и противников
class Ability_invise final: public Ability {
  nocopy(Ability_invise);
  struct Impl;
  Unique<Impl> impl {};

public:
  Ability_invise();
  ~Ability_invise();
  void update(Player& player, const double dt) override;
  inline void draw(Image& dst, const Vec offset) const override {}
  void powerup() override;
  utf32 name() const override;
  utf32 desc() const override;
};
