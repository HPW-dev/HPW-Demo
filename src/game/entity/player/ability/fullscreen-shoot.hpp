#pragma once
#include "ability.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"

// невидимость игрока, пуль и противников
class Ability_fullscreen_shoot final: public Ability {
  nocopy(Ability_fullscreen_shoot);
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Ability_fullscreen_shoot(CN<Player> player);
  ~Ability_fullscreen_shoot();
  void update(Player& player, const Delta_time dt) override;
  inline void draw(Image& dst, const Vec offset) const override {}
  void power_up() override;
  utf32 name() const override;
  utf32 desc() const override;
  CP<Sprite> icon() const override;
};
