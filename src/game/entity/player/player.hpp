#pragma once
#include "game/entity/collidable.hpp"
#include "ability/ability-entry.hpp"

class Player: public Collidable, Ability_entry {
  nocopy(Player);

public:
  hp_t hp_max {};
  hp_t energy {}; // энергия, тратится на выстрелы и щит
  hp_t energy_max {};
  hp_t m_fuel {}; // топливо. Если кончится, то геймовер
  hp_t m_fuel_max {};
  real default_force {}; // сопротивление при обычном движении
  real focus_force {}; // сопротивление при фокусировке

  Player();
  ~Player();
  void update(const Delta_time dt) override;
  void draw(Image& dst, const Vec offset) const override;
  void kill() override;
  virtual void sub_en(hp_t val);
}; // Player