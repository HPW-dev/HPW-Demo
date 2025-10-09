#pragma once
#include <utility>
#include <cassert>
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
  : _values{ values } {}

  constexpr inline Rnd_table(Values&& values) noexcept
  : _values{ std::move(values) } {}

  inline Value_cref push(Value_cref val) {
    _values.push_back(val);
    return *_values.back();
  }

  inline Value_cref push(Value_t&& val) {
    return _values.emplace_back(std::move(val));
  }

  constexpr inline Values_cref values() const { return _values; }

  inline Value_cref rnd_stable() const {
    cauto sz = _values.size();
    assert(sz > 0);
    if (sz == 1)
      return _values[0];
    _last_idx = rndu(sz-1);
    return _values.at(_last_idx); 
  }
  
  inline Value_cref rnd_fast() const {
    cauto sz = _values.size();
    assert(sz > 0);
    if (sz == 1)
      return _values[0];
    _last_idx = rndu_fast(sz-1);
    return _values.at(_last_idx);
  }

  // взять элемент по порядку
  inline Value_cref next() const {
    assert(!_values.empty());
    cauto sz = _values.size();
    if (sz == 1)
      return _values[0];
    _last_idx = (_last_idx + 1) % sz;
    return _values.at(_last_idx);
  }

private:
  Values _values {};
  mutable std::size_t _last_idx {}; // id предыдущего вернутого элемента
};
