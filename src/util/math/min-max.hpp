#pragma once
#include <cassert>

// хранит минимальное и максимальное значение
template <class T>
struct Minmax {
  T min {};
  T max {};

  inline bool check() const { return max >= min; }
  inline T diff() const { return max - min; }
};
