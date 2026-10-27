#pragma once
#include <limits>

template <class T>
constexpr auto num_max() { return std::numeric_limits<T>::max(); }

template <class T>
constexpr auto num_min() { return std::numeric_limits<T>::min(); }

template <class T>
constexpr auto max_prec() { return std::numeric_limits<T>::digits10; }
