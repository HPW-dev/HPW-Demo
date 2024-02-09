#include <cassert>
#include <random>
#include "rand-table-256.hpp"
#include "random.hpp"
#include "game/game-debug.hpp"

#ifdef STABLE_REPLAY
  #include "util/error.hpp"
  #include "util/hpw-util.hpp"
#endif

namespace {
  inline uint32_t seed {1};
  inline uint32_t rndr_fast_rpng {1};
  inline uint32_t rnd_fast_lcg_state {1};
  inline uint32_t rndu_fast_lcg_state {1};
  inline uint32_t rndb_lcg_state {1};
  inline auto rnd_gen {std::minstd_rand()};
  inline auto rndu_gen {std::minstd_rand()};
  inline auto rndr_gen {std::minstd_rand()};
  inline uint8_t table_idx {0};
}

/// узнаёт, можно ли вызывать эту функцию при hpw::allow_random_stable
inline void check_safe_random() {
  #ifdef STABLE_REPLAY
  if ( !hpw::allow_random_stable)
    error("рандом вызывается не там где надо");
  #endif
}


inline CN<uint8_t> table_rndb() {
  return rand_table_256[table_idx++];
}

inline uint RPNG(uint32_t& state) {
  state = 8'253'729u * state + 2'396'403u;
  state %= 32'767u;
  return state;
}

inline uint32_t lcg_parkmiller(uint32_t& state) {
  // Precomputed parameters for Schrage's method
  constexpr uint32_t M = 0x7fffffff;
  constexpr uint32_t A = 48271;
  constexpr uint32_t Q = M / A;    // 44488
  constexpr uint32_t R = M % A;    //  3399
  uint32_t div = state / Q;	// max: M / Q = A = 48,271
  uint32_t rem = state % Q;	// max: Q - 1     = 44,487
  int32_t s = rem * A;	// max: 44,487 * 48,271 = 2,147,431,977 = 0x7fff3629
  int32_t t = div * R;	// max: 48,271 *  3,399 =   164,073,129
  int32_t result = s - t;
  if (result < 0)
    result += M;
  return state = result;
}

/// прогрев генераторов
inline void init_generators() {
  ALLOW_STABLE_RAND
  cfor (_, 10u) {
    do_not_optimize(rndb());
    do_not_optimize(rnd());
    do_not_optimize(rndu());
    do_not_optimize(rndr());
    do_not_optimize(rndb_fast());
    do_not_optimize(rnd_fast());
    do_not_optimize(rndu_fast());
    do_not_optimize(rndr_fast());
  }
}

void set_rnd_seed(uint32_t new_seed) {
  #pragma omp critical(rnd_seed_section)
  {
    if (new_seed == 0)
      new_seed = 97'997u;
    seed = new_seed;
    srand(seed);
    rndr_fast_rpng = new_seed;
    rnd_fast_lcg_state = seed;
    rndu_fast_lcg_state = seed;
    rndb_lcg_state = seed;
    rnd_gen.seed(seed);
    rndr_gen.seed(seed);
    rndu_gen.seed(seed);
    table_idx = uint8_t(seed);
  } // omp critical

  init_generators();
}

uint8_t rndb() {
  check_safe_random();
  uint8_t ret;
  #pragma omp critical(rndb_section)
  { ret = lcg_parkmiller(rndb_lcg_state); }
  return ret;
}

int32_t rnd(int32_t rmin, int32_t rmax) {
  check_safe_random();
  if (rmin >= rmax)
    return rmin;
  std::uniform_int_distribution<int32_t> dist(rmin, rmax);
  int32_t ret;
  #pragma omp critical(rnd_section)
  { ret = dist(rnd_gen); }
  return ret;
}

uint32_t rndu(uint32_t rmax) {
  check_safe_random();
  if (rmax == 0)
    return 0;
  std::uniform_int_distribution<uint32_t> dist(0, rmax);
  uint32_t ret;
  #pragma omp critical(rndu_section)
  { ret = dist(rndu_gen); }
  return ret;
}

real rndr(real rmin, real rmax) {
  check_safe_random();
  if (rmin >= rmax)
    return rmin;
  std::uniform_real_distribution<real> dist(rmin, rmax);
  real ret;
  #pragma omp critical(rndr_section)
  { ret = dist(rndr_gen); }
  return ret;
}

uint32_t get_rnd_seed() {
  uint32_t ret;
  #pragma omp critical(rnd_seed_section)
  { ret = seed; }
  return ret;
}

uint8_t rndb_fast() { return table_rndb(); }
int32_t rnd_fast() { return lcg_parkmiller(rnd_fast_lcg_state); }
uint32_t rndu_fast() { return lcg_parkmiller(rndu_fast_lcg_state); }
real rndr_fast() { return RPNG(rndr_fast_rpng) * real(1.0 / 32767.0); }

int32_t rnd_fast(int32_t rmin, int32_t rmax) {
  return (rnd_fast() % (rmax + std::abs(rmin) + 1)) - std::abs(rmin);
}
uint32_t rndu_fast(uint32_t rmax) {
  return rndu_fast() % rmax;
}
real rndr_fast(real rmin, real rmax) {
  return (rndr_fast() * (rmax + std::abs(rmin))) - std::abs(rmin);
}
