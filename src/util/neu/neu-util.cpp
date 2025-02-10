#include <algorithm>
#include <cmath>
#include <numeric>
#include "neu-util.hpp"
#include "util/math/random.hpp"

namespace neu {

void randomize(Base& dst, Weight wmin, Weight wmax, bool allow_zero) {
  auto& weights = dst.weights();

  for (rauto w: weights) {
    w = rndr(wmin, wmax);

    if (!allow_zero && w == 0)
      w = 0.001;
  }
}

Weights softmax(cr<Weights> src) {
  Weights ret(src.size());
  std::transform(src.begin(), src.end(), ret.begin(), [](const Weight in)->Weight { return std::exp(in); });
  const Weight sum = std::accumulate(ret.begin(), ret.end(), Weight{0});

  for (rauto x: ret)
    x /= sum;

  return ret;
}

} // neu ns
