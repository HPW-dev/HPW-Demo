#pragma once
#include <cstdint>
#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/vector-types.hpp"
#include "util/mem-types.hpp"
#include "util/math/limit.hpp"
#include "util/math/num-types.hpp"

class Yaml;

// pge is plugin graphic effect

struct Param_pge {
  Str title {};
  Str description {};
  enum class Type { base, param_int, param_real, param_bool };
  Type type {Type::base};
  virtual void save(Yaml& dst) const;
  virtual void load(CN<Yaml> dst);
};

struct Param_pge_int: public Param_pge {
  std::int32_t* value {};
  std::int32_t min {num_min<std::int32_t>()};
  std::int32_t max {num_max<std::int32_t>()};
  std::int32_t speed_step {1};
  inline explicit Param_pge_int() { type = Param_pge::Type::param_int; }
  void save(Yaml& dst) const override;
  void load(CN<Yaml> dst) override;
};

struct Param_pge_real: public Param_pge {
  real* value {};
  real min {num_min<real>()};
  real max {num_max<real>()};
  real speed_step {1};
  inline explicit Param_pge_real() { type = Param_pge::Type::param_real; }
  void save(Yaml& dst) const override;
  void load(CN<Yaml> dst) override;
};

struct Param_pge_bool: public Param_pge {
  bool* value {};
  inline explicit Param_pge_bool() { type = Param_pge::Type::param_bool; }
  void save(Yaml& dst) const override;
  void load(CN<Yaml> dst) override;
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
CN<Str> get_cur_pge_description();