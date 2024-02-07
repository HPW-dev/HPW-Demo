#pragma once

/// вызывает функцию при деструкторе и конструкторе (для областей видимости)
class Scope final {
  using Func = void (*)();
  Func m_ctor_fn {}; /// функция при конструировании
  Func m_dtor_fn {}; /// функция при деструкторе
  
public:
  explicit Scope(Func&& ctor_fn, Func&& dtor_fn);
  ~Scope();
};

/// pixel per sec.
constexpr auto pps(auto in) { return in * 60; }
/// pixel per sec.
constexpr long double operator""_pps(long double in) { return in * 60.0l; }
/// pixel per sec.
constexpr unsigned long long int operator""_pps(unsigned long long int in) { return in * 60ull; }

/// x -> время ожидания в кадрах
constexpr double to_fps(double x) { return (1.0 / 60.0) * x; }
