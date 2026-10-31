#include "pch.hpp"
#include "random.hpp"
#include "rand-table-256.hpp"
#include "xorshift.hpp"

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
  explicit Rnd_gen_table(Seed seed) { _idx = seed.old_seed % 256; }
  result_type operator()() override {
    cauto a = rand_table_256[_idx++];
    cauto b = rand_table_256[_idx++];
    cauto c = rand_table_256[_idx++];
    cauto d = rand_table_256[_idx++];
    return (a << 24) | (b << 16) | (c << 8) | d;
  }
};

class Rnd_gen_rpng final: public Rnd_gen_base {
  u32_t _state;
public:
  explicit Rnd_gen_rpng(Seed seed) {
    _state = seed.old_seed;
    if (_state == 0)
      _state = 97997;
  }
  result_type operator()() override { 
    cauto a = RPNG(_state);
    cauto b = RPNG(_state);
    cauto c = RPNG(_state);
    return (a << 17) | (b << 2) | (c & 3);
  }
};

class Rnd_gen_mt19937 final: public Rnd_gen_base {
  std::mt19937 _gen;
public:
  explicit Rnd_gen_mt19937(Seed seed) { _gen.seed(seed.old_seed); }
  result_type operator()() override {
    static_assert (_gen.max() == Rnd_gen_base::max(), "mt19937.max() != Rnd_gen_base::max()");
    return _gen();
  }
};

class Rnd_gen_parkmiller final: public Rnd_gen_base {
  u32_t _state;
public:
  explicit Rnd_gen_parkmiller(Seed seed) { _state = seed.old_seed; }
  result_type operator()() override {
    cauto a = lcg_parkmiller(_state);
    cauto b = lcg_parkmiller(_state);
    return (a << 1) | (b & 1);
  }
};

class Rnd_gen_xorshift128 final: public Rnd_gen_base {
  xorshift128_state _state {};
public:
  explicit Rnd_gen_xorshift128(Seed seed) {
    _state.a ^= seed.old_seed;
    _state.b ^= seed.old_seed;
    _state.c ^= seed.old_seed;
    _state.d ^= seed.old_seed;
  }
  result_type operator()() override { return xorshift128(_state); }
};

// рандом из эмулятора A26
class Rnd_gen_a26 final : public Rnd_gen_base {
  uint16_t _state;

  inline uint8_t step_tia() {
    uint16_t bit = ((_state >> 4) ^ (_state >> 8)) & 1;
    _state = (_state << 1) | bit;
    return static_cast<uint8_t>(_state & 0xFF);
  }

public:
  inline explicit Rnd_gen_a26(Seed seed) { 
    _state = (seed.old_seed == 0)
      ? 0x1FF
      : (seed.old_seed & 0x1FF); 
  }
  result_type operator()() override {
    cauto a = step_tia();
    cauto b = step_tia();
    cauto c = step_tia();
    cauto d = step_tia();
    return (a << 24) | (b << 16) | (c << 8) | d;
  }
};

class Rnd_gen_fnv1a final : public Rnd_gen_base {
  u32_t _counter;
  // Классические константы для 32-битного FNV-1a
  static constexpr u32_t FNV_prime = 16777619u;
  static constexpr u32_t FNV_offset_basis = 2166136261u;

  inline u32_t hash_u32(u32_t value) {
    u32_t hash = FNV_offset_basis;

    hash ^= (value & 0xFF);
    hash *= FNV_prime;

    hash ^= ((value >> 8) & 0xFF);
    hash *= FNV_prime;

    hash ^= ((value >> 16) & 0xFF);
    hash *= FNV_prime;

    hash ^= ((value >> 24) & 0xFF);
    hash *= FNV_prime;

    return hash;
  }

public:
  explicit Rnd_gen_fnv1a(Seed seed) { _counter = seed.old_seed; }
  result_type operator()() override { return hash_u32(_counter++); }
};

using Rnd_gen_maker = std::function<Unique<Rnd_gen_base> ()>;
sconst std::unordered_map<Str, Rnd_gen_maker> _table {
  {"xorshift128", []{ return new_unique<Rnd_gen_xorshift128>(main_seed); }},
  {"Parkmiller", []{ return new_unique<Rnd_gen_parkmiller>(main_seed); }},
  {"mt19937", []{ return new_unique<Rnd_gen_mt19937>(main_seed); }},
  {"FNV1-a", []{ return new_unique<Rnd_gen_fnv1a>(main_seed); }},
  {"table", []{ return new_unique<Rnd_gen_table>(main_seed); }},
  {"RPNG", []{ return new_unique<Rnd_gen_rpng>(main_seed); }},
  {"a26", []{ return new_unique<Rnd_gen_a26>(main_seed); }},
};

Strs rnd_gens() {
  Strs ret;
  for (crauto [name, _]: _table)
    ret.push_back(name);
  return ret;
}

Stl_rnd make_rnd_gen(cr<Str> name) {
  try {
    return Stl_rnd{.gen = std::move(_table.at(name)())};
  } catch (...) {
    error("error while making random generator (" << name << ")");
  }

  return {}; // заглушка для анализатора
}
