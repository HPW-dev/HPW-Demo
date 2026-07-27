#pragma once
#include <type_traits>

// проверяет что есть стд'эшные методы
template <class T>
concept Std_container = requires (T x) {
  {x.begin()};
  {x.end()};
  {x.size()};
};

template <class T>
concept have_begin_end = requires(T x) {
  {x.begin()};
  {x.end()};
};

template <class T>
concept have_xy = requires { T::x; T::y; };
