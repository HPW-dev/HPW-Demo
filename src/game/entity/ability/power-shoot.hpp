#pragma once
#include "ability.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"

// выстреливает кучу пуль при большом количестве энергии 
class Ability_power_shoot final: public Ability {
  nocopy(Ability_power_shoot);
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Ability_power_shoot(CN<Player> player);
  ~Ability_power_shoot();
  void update(Player& player, const double dt) override;
  inline void draw(Image& dst, const Vec offset) const override {}
  void power_up() override;
  utf32 name() const override;
  utf32 desc() const override;
};
