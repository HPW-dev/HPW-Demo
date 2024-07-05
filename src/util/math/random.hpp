#pragma once
#include <cstdint>
#include "util/macro.hpp"
#include "util/math/limit.hpp"
#include "util/math/num-types.hpp"

void set_rnd_seed(std::uint32_t new_seed);
std::uint32_t get_rnd_seed();
// random byte
std::uint8_t rndb();
std::uint8_t rndb_fast();
// random i32
int32_t rnd(int32_t rmin=num_min<int32_t>(), int32_t rmax=num_max<int32_t>());
int32_t rnd_fast(int32_t rmin, int32_t rmax);
int32_t rnd_fast();
// random u32
std::uint32_t rndu(std::uint32_t rmax=num_max<std::uint32_t>());
std::uint32_t rndu_fast(std::uint32_t rmax);
std::uint32_t rndu_fast();
// random real
real rndr(real rmin=0, real rmax=1);
real rndr_fast(real rmin, real rmax);
real rndr_fast();
