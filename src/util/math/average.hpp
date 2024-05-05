#pragma once
#include <algorithm>
#include <numeric>
#include <deque>
#include "util/macro.hpp"

// Для получения среднего значения или медианы нескольких значений
template <class T, std::size_t size>
class Average final {
  std::deque<T> samples {};

public:
  inline Average(T filler = {})
  : samples(size) {
    std::fill(samples.begin(), samples.end(), filler);
  }

  // добавить значение
  inline void push(CN<T> x) {
    samples.pop_front();
    samples.push_back(x);
  }

  inline T get_average() const {
    return std::accumulate(samples.begin(), samples.end(), T(0)) / T(size);
  }

  inline T get_median() const {
    decltype(samples) copy = samples;
    std::sort(copy.begin(), copy.end());
    return copy.at(copy.size() / 2);
  }
}; // Average
