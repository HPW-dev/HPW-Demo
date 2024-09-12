#include <omp.h>
#include <cassert>
#include <cmath>
#include "noise.hpp"
#include "graphic/image/image.hpp"

void noise_2d(Image& dst, const Vec OFFSET, const int HARMONICS, const real STEP_START,
const real STEP_MUL, const real AMPLIFY, const real SCALE) {
  assert(dst);
  assert(HARMONICS > 1);
  assert(STEP_START > 1);
  assert(STEP_MUL > 1);
  assert(AMPLIFY > 0);
  assert(SCALE > 0);

  constexpr auto gen_noise = [](int x, int y)->real
    { return ((((x * 125'245) ^ (y * 142'124)) + 124'534) % 10'000) / 5'000.f - 1.f; };

  constexpr auto blerp = [](real c00, real c10, real c01, real c11, real tx, real ty)->real
    { return std::lerp(std::lerp(c00, c10, tx), std::lerp(c01, c11, tx), ty); };

  real step = STEP_START;
  Vector<real> heightmap(dst.size, 0);

  cfor (harmonic, HARMONICS) {
    cauto scale_x = step / dst.X;
    cauto scale_y = step / dst.Y;

    #pragma omp parallel for simd collapse(2)
    cfor (y, dst.Y)
    cfor (x, dst.X) {
      cauto xf = ((x / SCALE) + OFFSET.x) * scale_x;
      cauto yf = ((y / SCALE) + OFFSET.y) * scale_y;
      const int xi = std::floor(xf);
      const int yi = std::floor(yf);
      cauto noise_00 = gen_noise(xi+0, yi+0) * AMPLIFY;
      cauto noise_10 = gen_noise(xi+1, yi+0) * AMPLIFY;
      cauto noise_01 = gen_noise(xi+0, yi+1) * AMPLIFY;
      cauto noise_11 = gen_noise(xi+1, yi+1) * AMPLIFY;
      cauto height = blerp(noise_00, noise_10, noise_01, noise_11, xf - xi, yf - yi);
      heightmap[y * dst.X + x] += height; 
    }
    step *= STEP_MUL;
  }

  cfor (i, heightmap.size()) {
    cauto luma = (heightmap[i] + 1.f) * 0.5f;
    cauto color = Pal8::from_real(luma);
    dst[i] = color;
  }
}
