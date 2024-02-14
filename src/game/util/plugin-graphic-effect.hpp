#pragma once
#include <cstdint>
#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/vector-types.hpp"
#include "util/mem-types.hpp"
#include "util/math/limit.hpp"
#include "util/math/num-types.hpp"

// pge is plugin graphic effect

struct Param_pge {
  Str title {};
  Str description {};
  enum Type { base, param_int, param_real };
  Type type {base};
};

struct Param_pge_int: public Param_pge {
  std::int32_t* value {};
  std::int32_t min {num_min<std::int32_t>()};
  std::int32_t max {num_max<std::int32_t>()};
  std::int32_t speed_step {1};
  inline explicit Param_pge_int() { type = Param_pge::param_int; }
};

struct Param_pge_real: public Param_pge {
  real* value {};
  real min {num_min<real>()};
  real max {num_max<real>()};
  real speed_step {1};
  inline explicit Param_pge_real() { type = Param_pge::param_real; }
};

/// грузить графический эффект из .dll/.so файл
void load_pge(Str libname);
/// применяет графический эффект к кадру
void apply_pge(const uint32_t state);
/// выключает текущий графический эффект
void disable_pge();
void load_pge_from_config();
void save_pge_to_config();
/// получить настраиваемые параметры из граф-о эффекта
CN< Vector<Shared<Param_pge>> > get_pge_params();
CN<Str> get_cur_pge_path();
CN<Str> get_cur_pge_name();