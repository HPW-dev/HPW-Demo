#pragma once
#include "game/entity/collidable.hpp"
#include "ability/ability-entry.hpp"

class Player: public Collidable, public Ability_entry {
  nocopy(Player);
  void update_sound();

public:
  hp_t hp_max {};
  hp_t energy {}; // энергия, тратится на выстрелы и щит
  hp_t energy_max {};
  hp_t fuel {}; // топливо. Если кончится, то геймовер
  hp_t fuel_max {};
  real default_force {}; // сопротивление при обычном движении
  real focus_force {}; // сопротивление при фокусировке
  real energy_consumption {1.}; // множитель затрат энергии
  real bullet_speed_amp {1.}; // множитель скорости пуль
  real attack_cooldown_amp {1.}; // множитель скорости перезарядки

  Player();
  ~Player();
  void update(const Delta_time dt) override;
  void draw(Image& dst, const Vec offset) const override;
  void process_kill() override;
  virtual void sub_en(hp_t val);
}; // Player