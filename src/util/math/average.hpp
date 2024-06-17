#pragma once
#include <algorithm>
#include <numeric>
#include <deque>
#include <type_traits>
#include "util/macro.hpp"

// заполняется значениями, когда их больше MAX_SAMPLES, заполняется по кругу
template <class T, std::size_t MAX_SAMPLES>
struct Ring_buffer {
  std::deque<T> samples {};

  inline explicit Ring_buffer() {
    static_assert(std::is_arithmetic_v<T> == true);
  }

  // добавить значение
  inline void move(T&& x) {
    // вставлять значение по кругу
    if (samples.size() >= MAX_SAMPLES) {
      samples.pop_front();
      samples.emplace_back(x);
    } else { // добавлять новое значение
      samples.emplace_back(x);
    }
  }
}; // Average_base

// поиск среднего значения
template <class T, std::size_t MAX_SAMPLES>
class Average final {
 private:
  Ring_buffer<T, MAX_SAMPLES> core {};

 public:
  inline T operator()() const {
    return_if (core.samples.empty(), {});
    return std::accumulate(core.samples.begin(), core.samples.end(), T{0}) / core.samples.size();
  }

  inline void move(T&& x) { core.move(std::move(x)); }
};

// поиск медианного значения
template <class T, std::size_t MAX_SAMPLES>
class Median final {
 private:
  Ring_buffer<T, MAX_SAMPLES> core {};

 public:
  inline T operator()() {
    return_if (core.samples.empty(), {});
    return_if (core.samples.size() == 1, core.samples[0]);
    return_if (core.samples.size() == 2, (core.samples[0] + core.samples[1]) * 0.5);
    std::sort(core.samples.begin(), core.samples.end());
    if (core.samples.size() % 2 == 0) {
      return 0.5 * (
        core.samples.at((core.samples.size() / 2) - 1) +
        core.samples.at(core.samples.size() / 2)
      );
    }
    return core.samples.at(core.samples.size() / 2);
  }

  inline void move(T&& x) { core.move(std::move(x)); }
};
