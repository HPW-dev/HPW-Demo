#pragma once
#include <limits>

template <class T>
auto num_max() { return std::numeric_limits<T>::max(); }

template <class T>
auto num_min() { return std::numeric_limits<T>::min(); }

template <class T>
auto max_prec() { return std::numeric_limits<T>::digits10; }
