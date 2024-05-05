#pragma once
#include "game/entity/entity-type.hpp"
#include "util/str.hpp"

class Yaml;
class Collidable;

// Инфа о хп и дамаге для загрузчика entity
struct Collidable_info {
  hp_t hp {};
  hp_t dmg {};
  Str exolosion_name {}; // с каким взрывом объект уничтожится
  bool ignore_enemy {};
  bool ignore_bullet {};
  bool ignore_self_type {};
  bool ignore_master {};
  bool ignore_scatter {};
  bool ignore_player {};

  void load(CN<Yaml> node);
  void accept(Collidable& dst);
};
