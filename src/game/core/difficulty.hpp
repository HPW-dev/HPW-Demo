#pragma once
#include "util/num-types.hpp"

enum class Difficulty: byte {
  easy,
  normal,
  hardcore
};

namespace hpw { inline Difficulty difficulty {Difficulty::normal}; }
