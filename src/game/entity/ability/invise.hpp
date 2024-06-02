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
  explicit Ability_invise(CN<Player> player);
  ~Ability_invise();
  void update(Player& player, const Delta_time dt) override;
  inline void draw(Image& dst, const Vec offset) const override {}
  void power_up() override;
  utf32 name() const override;
  utf32 desc() const override;
  CP<Sprite> icon() const override;
};
