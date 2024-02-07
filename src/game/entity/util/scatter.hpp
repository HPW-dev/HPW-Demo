#pragma once
#include "util/math/vec.hpp"

class Entity;

/// параметры взрывной волны
struct Scatter {
  enum class Type {
    outside = 0, /// отталкивает
    inside, /// притягивает
    random, /// разлёт в случайные стороны
  };

  Vec pos {};
  double range {}; /// дальность действия эффекта
  double power {}; /// сила откидывания (pps)
  Type type {};
  bool disable_shake {false}; /// выключает тряску

  void accept(Entity& dst) const; /// применяет волну на объекте
}; // Scatter
