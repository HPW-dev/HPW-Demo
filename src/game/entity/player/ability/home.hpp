#pragma once
#include "ability.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"

// Захват противников через Домик
class Ability_home final: public Ability {
  nocopy(Ability_home);
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Ability_home(cr<Player> player);
  ~Ability_home();
  void update(Player& player, const Delta_time dt) override;
  void draw(Image& dst, const Vec offset) const override;
  void power_up() override;
  utf32 name() const override;
  utf32 desc() const override;
  cp<Sprite> icon() const override;
};
