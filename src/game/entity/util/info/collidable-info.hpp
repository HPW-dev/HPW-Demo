#pragma once
#include <thirdparty/include/nlohmann/json_fwd.hpp>
#include "game/entity/entity-type.hpp"
#include "util/str.hpp"

class Collidable;

// Инфа о хп и дамаге для загрузчика entity
struct Collidable_info {
  hp_t hp {};
  hp_t dmg {};
  Str explosion_name {}; // с каким взрывом объект уничтожится
  bool ignore_enemy {};
  bool ignore_bullet {};
  bool ignore_self_type {};
  bool ignore_master {};
  bool ignore_player {};

  void load(cr<nlohmann::json> node);
  void accept(Collidable& dst);
};
