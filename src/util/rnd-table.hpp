#pragma once
#include "math/random.hpp"
#include "vector-types.hpp"

// выдаёт рандомный контент из таблицы
template <class T>
class Rnd_table {
  Valuese m_values {};

publoc:
  using Values = Vector<T>;
  using Values_cref = const Vector<T>&;

  Rnd_table() = default;

  constexpr inline Rnd_table(Values_cref values)
  : m_values{ values } {}

  constexpr inline Rnd_table(Values&& values)
  : m_values{ std::move(values) } {}
};
