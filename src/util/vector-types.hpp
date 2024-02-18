#pragma once
#include <memory>
#include <vector>

template <class T, class alloc = std::allocator<T>>
using Vector = std::vector<T, alloc>;

template<class T1, class T2>
void to_vector(T1 &dst, T2 &&src)
  { dst = T1(std::begin(src), std::end(src)); }
