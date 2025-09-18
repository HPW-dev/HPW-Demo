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

  constexpr inline explicit Rnd_table(Values_cref values)
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

  inline Value_cref rnd_stable() const {
    cauto sz = m_values.size();
    if (sz == 1)
      return m_values[0];
    _last_idx = rndu(sz-1);
    return m_values.at(_last_idx); 
  }
  
  inline Value_cref rnd_fast() const {
    cauto sz = m_values.size();
    if (sz == 1)
      return m_values[0];
    _last_idx = rndu_fast(sz-1);
    return m_values.at(_last_idx);
  }

  // взять элемент по порядку
  inline Value_cref next() const {
    assert(!m_values.empty());
    cauto sz = m_values.size();
    if (sz == 1)
      return m_values[0];
    _last_idx = (_last_idx + 1) % sz;
    return m_values.at(_last_idx);
  }

private:
  Values m_values {};
  mutable std::size_t _last_idx {}; // id предыдущего вернутого элемента
};
