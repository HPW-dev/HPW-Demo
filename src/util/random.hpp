#pragma once
#include "util/mem-types.hpp"
#include "util/num-types.hpp"
#include "util/macro.hpp"
#include "util/limit.hpp"
#include "util/str.hpp"

struct Seed {
  u32_t old_seed {97997}; // TODO сделать другой большой сид
};

// База для фабрики генераторов рандома
struct Rnd_gen_base {
  // должен быть равен самому большому типу выдаваемому генераторами
  using result_type = u32_t;

  virtual ~Rnd_gen_base() = default;
  staticx result_type min() { return 0; }
  staticx result_type max() { return num_max<result_type>(); }
  virtual result_type operator()() = 0;
};

// Обёртка для STL-friendly генераторов
struct Stl_rnd {
  using result_type = Rnd_gen_base::result_type;
  Unique<Rnd_gen_base> gen {}; // задаётся в make_rnd_gen

  staticx result_type min() { return Rnd_gen_base::min(); }
  staticx result_type max() { return Rnd_gen_base::max(); }
  inline result_type operator()() { return (*gen)(); }
};

// фабрика генераторов рандома
[[nodiscard]] Stl_rnd make_rnd_gen(cr<Str> name);
[[nodiscard]] Strs rnd_gens(); // список всех доступных генераторов рандома

void set_rnd_seed(Seed seed);
u32_t get_rnd_seed();

// random byte
byte rndb();
byte rndb_fast();

// random i32
i32_t rnd(i32_t rmin=num_min<i32_t>(), i32_t rmax=num_max<i32_t>());
i32_t rnd_fast(i32_t rmin, i32_t rmax);
i32_t rnd_fast();

// random u32
u32_t rndu(u32_t rmax=num_max<u32_t>());
u32_t rndu_fast(u32_t rmax);
u32_t rndu_fast();

// random real
real rndr(real rmin=0, real rmax=1);
real rndr_fast(real rmin=0, real rmax=1);
real rndr_graphic(real rmin=0, real rmax=1);
