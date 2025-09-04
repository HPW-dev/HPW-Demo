#pragma once
#include <algorithm>
#include "util/vector-types.hpp"
#include "util/math/num-types.hpp"
#include "util/str-util.hpp"
#include "util/macro.hpp"

// аккумулятор статистики - среднее, медиана, минимум и т.д.
template <class T>
struct Full_stats {
  std::optional<T> min {};
  std::optional<T> max {};
  std::optional<T> last {};
  mutable Vector<T> samples {};

  inline void clear() {
    min.reset();
    max.reset();
    last.reset();
    samples.clear();
  }

  inline void push(const T val) {
    last = val;
    if (!min) {
      min = val;
      max = val;
    } else {
      min = std::min(*min, val);
      max = std::max(*max, val);
    }
    samples.push_back(val);
  }

  inline double average() const {
    ret_if (samples.empty(), 0);
    return std::accumulate(samples.begin(), samples.end(), T{}) / T(samples.size());
  }

  inline double median() const {
    ret_if (samples.empty(), {});
    cauto SZ = samples.size();
    ret_if (SZ == 1, samples[0]);
    ret_if (SZ == 2, (samples[0] + samples[1]) * 0.5);

    std::sort(samples.begin(), samples.end());
    ret_if (
      SZ % 2 == 0,
      0.5 * (samples.at((SZ / 2) - 1) + samples.at(SZ / 2)) );
    return samples.at(SZ / 2);
  }

  inline Str str(uint precission = 1, bool endl = true) const {
    Str ret;
    cauto EOL = (endl ? ";\n" : "; ");
    if (last) ret += "Last " + n2s(*last, precission) + EOL;
    if (min)  ret += "Min. " + n2s(*min, precission)  + EOL;
    if (max)  ret += "Max. " + n2s(*max, precission)  + EOL;
    ret += "Med. " + n2s(median(), precission) + EOL;
    ret += "Avr. " + n2s(average(), precission) + EOL;
    return ret;
  }
}; // Full_stats
