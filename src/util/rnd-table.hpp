#pragma once
#include <utility>
#include "math/random.hpp"
#include "vector-types.hpp"

// класс для случайных значений из списка
template <class T>
class Rnd_table {
public:
  using Value_t = T;
  using Value_cref = const T&;
  using Values = Vector<T>;
  using Values_cref = const Vector<T>&;

  Rnd_table() = default;

  constexpr inline Rnd_table(Values_cref values)
  : m_values{ values } {}

  constexpr inline Rnd_table(Values&& values) noexcept
  : m_values{ std::move(values) } {}

  inline Value_cref push(Value_cref val) {
    m_values.push_back(val);
    return *m_values.back();
  }

  inline Value_cref push(Value_t&& val) {
    return m_values.emplace_back(std::move(val));
  }

  constexpr inline Values_cref values() const { return m_values; }

  inline Value_cref rnd_stable() const
    { return m_values.at( rndu(m_values.size()) ); }
  
  inline Value_cref rnd_fast() const {
    cauto sz = m_values.size();
    if (sz == 1)
      return m_values[0];
    return m_values.at( rndu_fast(sz) );
  }

private:
  Values m_values {};
};
