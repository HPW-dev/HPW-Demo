#pragma once
#include "collidable.hpp"

/// способности, которые может выбрать игрок
struct Ability {
  bool power_shoot : 1 {}; /// мощный выстрел при накоплении энергии
  bool invisible : 1 {}; /// абсолютная невидимость для врагов и игрока
  bool graze_en_regen : 1 {}; /// реген энергии при грейзе
  bool stillness_en_regen : 1 {}; /// реген энергии при неподвижности
  bool shield : 1 {}; /// щит сейвит игрока и тратит энергию об пули
  bool speedup_move : 1 {}; /// движение во все стороны одинаково быстрое, но тратит EN
  bool direct_shoot : 1 {}; /// все атаки становятся прямыми
  bool speedup_shoot : 1 {}; /// все атаки ускоряются и ускоряется реген EN
};

class Player: public Collidable {
  nocopy(Player);

public:
  Ability ability {};
  hp_t hp_max {};
  hp_t energy {}; /// энергия, тратится на выстрелы и щит
  hp_t energy_max {};
  hp_t fuel {}; /// топливо. Если кончится, то геймовер
  hp_t fuel_max {};
  real default_force {}; /// сопротивление при обычном движении
  real focus_force {}; /// сопротивление при фокусировке

  Player();
  ~Player();
};