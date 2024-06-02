#pragma once
#include "ability.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"

// повышение скорости за счёт энергии
class Ability_speedup final: public Ability {
  nocopy(Ability_speedup);
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Ability_speedup(CN<Player> player);
  ~Ability_speedup();
  void update(Player& player, const Delta_time dt) override;
  inline void draw(Image& dst, const Vec offset) const override {}
  void power_up() override;
  utf32 name() const override;
  utf32 desc() const override;
  CP<Sprite> icon() const override;
};
