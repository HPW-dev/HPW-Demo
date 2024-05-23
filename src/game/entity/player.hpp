#pragma once
#include "collidable.hpp"

class Ability;

class Player: public Collidable {
  nocopy(Player);

public:
  Vector<Shared<Ability>> capabilities {}; // способности игрока
  hp_t hp_max {};
  hp_t energy {}; // энергия, тратится на выстрелы и щит
  hp_t energy_max {};
  hp_t m_fuel {}; // топливо. Если кончится, то геймовер
  hp_t m_fuel_max {};
  real default_force {}; // сопротивление при обычном движении
  real focus_force {}; // сопротивление при фокусировке

  Player();
  ~Player();
  void update(double dt) override;
  // добавить игроку новую способность
  void move_ability(Shared<Ability>&& ability);
}; // Player