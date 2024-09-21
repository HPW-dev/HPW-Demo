#pragma once

//#define RND_LOG
#ifdef RND_LOG
#include <source_location>
#endif

#include <cstdint>
#include "util/macro.hpp"
#include "util/math/limit.hpp"
#include "util/math/num-types.hpp"

// аргумент для дебажного вывода места вызова рандома
#ifdef RND_LOG
#define RND_SRC_LOC_ARG , const std::source_location location
#define RND_SRC_LOC_ARG_2 const std::source_location location
#define RND_SRC_LOC_ARG_DEF RND_SRC_LOC_ARG = std::source_location::current()
#define RND_SRC_LOC_ARG_2_DEF RND_SRC_LOC_ARG_2 = std::source_location::current()
#else
#define RND_SRC_LOC_ARG
#define RND_SRC_LOC_ARG_2
#define RND_SRC_LOC_ARG_DEF
#define RND_SRC_LOC_ARG_2_DEF
#endif

void set_rnd_seed(std::uint32_t new_seed RND_SRC_LOC_ARG_DEF);
std::uint32_t get_rnd_seed();
// random byte
std::uint8_t rndb(RND_SRC_LOC_ARG_2_DEF);
std::uint8_t rndb_fast();
// random i32
int32_t rnd(int32_t rmin=num_min<int32_t>(), int32_t rmax=num_max<int32_t>() RND_SRC_LOC_ARG_DEF);
int32_t rnd_fast(int32_t rmin, int32_t rmax);
int32_t rnd_fast();
// random u32
std::uint32_t rndu(std::uint32_t rmax=num_max<std::uint32_t>() RND_SRC_LOC_ARG_DEF);
std::uint32_t rndu_fast(std::uint32_t rmax);
std::uint32_t rndu_fast();
// random real
real rndr(real rmin=0, real rmax=1 RND_SRC_LOC_ARG_DEF);
real rndr_fast(real rmin, real rmax);
real rndr_fast();
