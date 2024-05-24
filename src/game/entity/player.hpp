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
  // добавить игроку новую способность
  Ability* move_ability(Shared<Ability>&& ability);
  // проверить что есть способность нужного типа
  Ability* find_ability(const std::size_t type_id) const;
  void update(double dt) override;
  void draw(Image& dst, const Vec offset) const override;
  void kill() override;
  virtual void sub_en(hp_t val);
}; // Player