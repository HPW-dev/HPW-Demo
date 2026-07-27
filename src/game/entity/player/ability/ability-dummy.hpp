#pragma once
#include "ability.hpp"

class Ability_dummy: public Ability {
public:
  Ability_dummy();
  ~Ability_dummy();
  Ability_id id() const override;
  void draw_bg(Image& dst, const Vec offset) override;
  void draw_fg(Image& dst, const Vec offset) override;
  void update(Delta_time dt) override;
  void on_upgrade() override;
  void on_downgrade() override;
};
