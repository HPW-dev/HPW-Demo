#include <cassert>
#include "edge-detect.hpp"
#include "graphic/image/image.hpp"

Image edge_detect(CN<Image> src, const real sensivity) {
  assert(src.X > 2);
  assert(src.Y > 2);
  assert(sensivity >= 0);
  assert(sensivity <= 1);

  Image ret(src.X, src.Y, Pal8::black);
  cfor (y, src.Y-1)
  cfor (x, src.X-1) {
    cauto p00 = src(x+0, y+0).to_real();
    cauto p10 = src(x+1, y+0).to_real();
    cauto p01 = src(x+1, y+0).to_real();
    if (std::abs(p00 - p10) >= sensivity)
      ret(x, y) = Pal8::white;
    if (std::abs(p00 - p01) >= sensivity)
      ret(x, y) = Pal8::white;
  }

  return ret;
}
