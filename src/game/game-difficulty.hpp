#pragma once

enum class Difficulty {
  easy,
  normal,
  hardcore
};

namespace hpw { inline Difficulty difficulty {Difficulty::normal}; }
