#include "pch.hpp"
#include "random.hpp"
#include "rand-table-256.hpp"

namespace {
  inline Seed main_seed {};
  inline u32_t seed {1};
  inline u32_t rndr_fast_rpng {1};
  inline u32_t rnd_fast_lcg_state {1};
  inline u32_t rndu_fast_lcg_state {1};
  inline u32_t rndb_lcg_state {1};
  inline auto rnd_gen {std::minstd_rand()};
  inline auto rndu_gen {std::minstd_rand()};
  inline auto rndr_gen {std::minstd_rand()};
  inline auto rndr_graphic_gen {std::minstd_rand()};
  inline byte table_idx {0};
}

inline cr<byte> table_rndb() {
  return rand_table_256[table_idx++];
}

inline uint RPNG(u32_t& state) {
  state = 8'253'729u * state + 2'396'403u;
  state %= 32'767u;
  return state;
}

inline u32_t lcg_parkmiller(u32_t& state) {
  // Precomputed parameters for Schrage's method
  constexpr u32_t M = 0x7fffffff;
  constexpr u32_t A = 48271;
  constexpr u32_t Q = M / A;    // 44488
  constexpr u32_t R = M % A;    //  3399
  u32_t div = state / Q;	// max: M / Q = A = 48,271
  u32_t rem = state % Q;	// max: Q - 1     = 44,487
  i32_t s = rem * A;	// max: 44,487 * 48,271 = 2,147,431,977 = 0x7fff3629
  i32_t t = div * R;	// max: 48,271 *  3,399 =   164,073,129
  i32_t result = s - t;
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

void set_rnd_seed(Seed s) {
  #pragma omp critical(rnd_seed_section)
  {
    seed = s.old_seed;
    if (seed == 0) {
      log_warning << "Seed 0 is bad, using seed 97997...";
      main_seed.old_seed = seed = 97'997u;
    }
    main_seed = s;
    srand(seed);
    rndr_fast_rpng = seed;
    rnd_fast_lcg_state = seed;
    rndu_fast_lcg_state = seed;
    rndb_lcg_state = seed;
    rnd_gen.seed(seed);
    rndr_gen.seed(seed);
    rndr_graphic_gen.seed(seed);
    rndu_gen.seed(seed);
    table_idx = seed % 256;
  } // omp critical

  init_generators();
}

byte rndb() {
  byte ret;
  #pragma omp critical(rndb_section)
  {
    ret = lcg_parkmiller(rndb_lcg_state);
  }
  return ret;
}

i32_t rnd(i32_t rmin, i32_t rmax) {
  ret_if (rmin >= rmax, rmin);
  std::uniform_int_distribution<i32_t> dist(rmin, rmax);
  i32_t ret;
  #pragma omp critical(rnd_section)
  {
    ret = dist(rnd_gen);
  }
  return ret;
}

u32_t rndu(u32_t rmax) {
  return_if (rmax == 0, 0);
  std::uniform_int_distribution<u32_t> dist(0, rmax);
  u32_t ret;
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

u32_t get_rnd_seed() {
  u32_t ret;
  #pragma omp critical(rnd_seed_section)
  {
    ret = seed;
  }
  return ret;
}

byte rndb_fast() { return table_rndb(); }
i32_t rnd_fast() { return lcg_parkmiller(rnd_fast_lcg_state); }
u32_t rndu_fast() { return lcg_parkmiller(rndu_fast_lcg_state); }

i32_t rnd_fast(i32_t rmin, i32_t rmax) {
  cauto div = rmax - rmin + 1;
  assert(div);
  return rmin + (rnd_fast() % div);
}

u32_t rndu_fast(u32_t rmax) {
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

// ------------------[ STL-френдли рандомы ]------------------------------------

class Rnd_gen_table final: public Rnd_gen_base {
  uint8_t _idx;
public:
  inline explicit Rnd_gen_table(Seed seed) { _idx = seed.old_seed % 256; }
  inline result_type next() {
    cauto part1 = rand_table_256[_idx++];
    cauto part2 = rand_table_256[_idx++];
    cauto part3 = rand_table_256[_idx++];
    cauto part4 = rand_table_256[_idx++];
    return part1 | (part2 << 8) | (part3 << 16) | (part3 << 24);
  }
};

class Rnd_gen_rpng final: public Rnd_gen_base {
  u32_t _state;
public:
  inline explicit Rnd_gen_rpng(Seed seed) { _state = seed.old_seed; }
  inline result_type next() {
    cauto part1 = RPNG(_state);
    cauto part2 = RPNG(_state);
    cauto part3 = RPNG(_state);
    return part1 | (part2 << 15) | (part3 << 30);
  }
};

class Rnd_gen_mt19937 final: public Rnd_gen_base {
  std::mt19937 _gen;
  std::uniform_int_distribution<u32_t> _dist {
    Rnd_gen_base::min(),
    Rnd_gen_base::max()
  };
public:
  inline explicit Rnd_gen_mt19937(Seed seed) { _gen.seed(seed.old_seed); }
  inline result_type next() { return _dist(_gen); }
};

class Rnd_gen_parkmiller final: public Rnd_gen_base {
  u32_t _state;
public:
  inline explicit Rnd_gen_parkmiller(Seed seed) { _state = seed.old_seed; }
  inline result_type next() {
    return
      ((lcg_parkmiller(_state) & 1) << 31) |
        lcg_parkmiller(_state); 
  }
};

using Rnd_gen_maker = std::function<Unique<Rnd_gen_base> ()>;
sconst std::unordered_map<Str, Rnd_gen_maker> _table {
  {"table", []{ return new_unique<Rnd_gen_table>(main_seed); }},
  {"RPNG", []{ return new_unique<Rnd_gen_rpng>(main_seed); }},
  {"mt19937", []{ return new_unique<Rnd_gen_mt19937>(main_seed); }},
  {"parkmiller", []{ return new_unique<Rnd_gen_parkmiller>(main_seed); }},
};

Strs rnd_gens() {
  Strs ret;
  for (crauto [name, _]: _table)
    ret.push_back(name);
  return ret;
}

Unique<Rnd_gen_base> make_rnd_gen(cr<Str> name) {
  try {
    return _table.at(name)();
  } catch (...) {
    error("error while making random generator (" << name << ")");
  }

  return {}; // заглушка для анализатора
}
