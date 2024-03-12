#pragma once
#include "util/math/num-types.hpp"

enum class Difficulty: byte {
  easy,
  normal,
  hardcore
};

namespace hpw { inline Difficulty difficulty {Difficulty::normal}; }
