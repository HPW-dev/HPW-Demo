#include "neu-util.hpp"
#include "util/math/random.hpp"

namespace neu {

void randomize(Base& dst, Weight wmin, Weight wmax, bool allow_zero) {
  auto& weights = dst.hiden_weights();

  for (rauto w: weights) {
    w = rndr(wmin, wmax);

    if (!allow_zero && w == 0)
      w = 0.001;
  }
}

} // neu ns
