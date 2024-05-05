#pragma once
#include "collidable.hpp"

// способности, которые может выбрать игрок
enum class Ability {
  power_shoot, // мощный выстрел при накоплении энергии
  invisible, // абсолютная невидимость для врагов и игрока
  graze_en_regen, // реген энергии при грейзе
  stillness_en_regen, // реген энергии при неподвижности
  shield, // щит сейвит игрока и тратит энергию об пули
  speedup_move, // движение во все стороны одинаково быстрое, но тратит EN
  direct_shoot, // все атаки становятся прямыми
  speedup_shoot, // все атаки ускоряются и ускоряется реген EN, но повышается стоимость атак
  home, // похищение через крышу домика
};

class Player: public Collidable {
  nocopy(Player);

public:
  Vector<Ability> capabilities {};
  hp_t hp_max {};
  hp_t energy {}; // энергия, тратится на выстрелы и щит
  hp_t energy_max {};
  hp_t m_fuel {}; // топливо. Если кончится, то геймовер
  hp_t m_fuel_max {};
  real default_force {}; // сопротивление при обычном движении
  real focus_force {}; // сопротивление при фокусировке

  Player();
  ~Player();
  // проверить что у игрока есть определённая способность
  bool check_capability(const Ability ability) const;
  void update(double dt) override;
};