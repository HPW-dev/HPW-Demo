#include "pch.hpp"
#include "random.hpp"
#include "rand-table-256.hpp"

namespace {
  inline std::uint32_t seed {1};
  inline std::uint32_t rndr_fast_rpng {1};
  inline std::uint32_t rnd_fast_lcg_state {1};
  inline std::uint32_t rndu_fast_lcg_state {1};
  inline std::uint32_t rndb_lcg_state {1};
  inline auto rnd_gen {std::minstd_rand()};
  inline auto rndu_gen {std::minstd_rand()};
  inline auto rndr_gen {std::minstd_rand()};
  inline auto rndr_graphic_gen {std::minstd_rand()};
  inline std::uint8_t table_idx {0};
}

inline cr<std::uint8_t> table_rndb() {
  return rand_table_256[table_idx++];
}

inline uint RPNG(std::uint32_t& state) {
  state = 8'253'729u * state + 2'396'403u;
  state %= 32'767u;
  return state;
}

inline std::uint32_t lcg_parkmiller(std::uint32_t& state) {
  // Precomputed parameters for Schrage's method
  constexpr std::uint32_t M = 0x7fffffff;
  constexpr std::uint32_t A = 48271;
  constexpr std::uint32_t Q = M / A;    // 44488
  constexpr std::uint32_t R = M % A;    //  3399
  std::uint32_t div = state / Q;	// max: M / Q = A = 48,271
  std::uint32_t rem = state % Q;	// max: Q - 1     = 44,487
  int32_t s = rem * A;	// max: 44,487 * 48,271 = 2,147,431,977 = 0x7fff3629
  int32_t t = div * R;	// max: 48,271 *  3,399 =   164,073,129
  int32_t result = s - t;
  if (result < 0)
    result += M;
  return state = result;
}

// прогрев генераторов
inline void init_generators() {
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

void set_rnd_seed(std::uint32_t new_seed) {
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
    rndr_graphic_gen.seed(seed);
    rndu_gen.seed(seed);
    table_idx = std::uint8_t(seed);
  } // omp critical

  init_generators();
}

std::uint8_t rndb() {
  std::uint8_t ret;
  #pragma omp critical(rndb_section)
  {
    ret = lcg_parkmiller(rndb_lcg_state);
  }
  return ret;
}

int32_t rnd(int32_t rmin, int32_t rmax) {
  ret_if (rmin >= rmax, rmin);
  std::uniform_int_distribution<int32_t> dist(rmin, rmax);
  int32_t ret;
  #pragma omp critical(rnd_section)
  {
    ret = dist(rnd_gen);
  }
  return ret;
}

std::uint32_t rndu(std::uint32_t rmax) {
  return_if (rmax == 0, 0);
  std::uniform_int_distribution<std::uint32_t> dist(0, rmax);
  std::uint32_t ret;
  #pragma omp critical(rndu_section)
  { 
    ret = dist(rndu_gen);
  }
  return ret;
}

real rndr(real rmin, real rmax) {
  return_if (rmin >= rmax, rmin);
  std::uniform_real_distribution<real> dist(rmin, rmax);
  real ret;
  #pragma omp critical(rndr_section)
  {
    ret = dist(rndr_gen);
  }
  return ret;
}

std::uint32_t get_rnd_seed() {
  std::uint32_t ret;
  #pragma omp critical(rnd_seed_section)
  {
    ret = seed;
  }
  return ret;
}

std::uint8_t rndb_fast() { return table_rndb(); }
int32_t rnd_fast() { return lcg_parkmiller(rnd_fast_lcg_state); }
std::uint32_t rndu_fast() { return lcg_parkmiller(rndu_fast_lcg_state); }

int32_t rnd_fast(int32_t rmin, int32_t rmax) {
  cauto div = rmax - rmin + 1;
  assert(div);
  return rmin + (rnd_fast() % div);
}

std::uint32_t rndu_fast(std::uint32_t rmax) {
  ++rmax;
  assert(rmax);
  return rndu_fast() % rmax;
}

real rndr_fast(real rmin, real rmax) {
  cauto rd = RPNG(rndr_fast_rpng) * real(1.0 / 32767.0);
  return rmin + rd * (rmax - rmin);
}

real rndr_graphic(real rmin, real rmax) {
  return_if (rmin >= rmax, rmin);
  std::uniform_real_distribution<real> dist(rmin, rmax);
  return dist(rndr_graphic_gen);
}

using Rnd_gen_maker = std::function<Unique<Rnd_gen_base> ()>;
sconst std::unordered_map<Str, Rnd_gen_maker> table {
  // TODO
};

Unique<Rnd_gen_base> make_rnd_gen(cr<Str> name) {
  try {
    return table.at(name)();
  } catch (...) {
    error("error while making random generator (" << name << ")");
  }

  return {}; // заглушка для анализатора
}
