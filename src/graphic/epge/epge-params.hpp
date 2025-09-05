#pragma once
#include "util/mem-types.hpp"
#include "util/str.hpp"
#include "util/vector-types.hpp"
#include "util/macro.hpp"
#include "util/unicode.hpp"

namespace epge {

// описание параметра для настроек плагина
class Param {
  Str _title_id {}; // сохраняемое название для опции
  utf32 _title {}; // отображаемое название опции
  utf32 _desc {}; // коммент с пояснением параметра

public:
  explicit Param(cr<Str> title_id, cr<utf32> title, cr<utf32> desc);
  virtual ~Param() = default;
  virtual void set_value(cr<Str> val) = 0;
  virtual Str get_value() const = 0;
  virtual inline void set_max(cr<Str> val) {} // максимальный предел параметра
  virtual inline void set_min(cr<Str> val) {} // минимальный предел параметра
  virtual inline Str get_max() const { return {}; }
  virtual inline Str get_min() const { return {}; }
  virtual inline void set_step(cr<Str> step) {} // шаг с которым меняется параметр
  virtual inline Str get_step() const { return {}; }
  virtual inline void set_fast_step(cr<Str> step) {} // ускоренное изменение параметра
  virtual inline Str get_fast_step() const { return {}; }
  virtual void plus_value() = 0;
  virtual void minus_value() = 0;
  virtual inline void plus_value_fast() {}
  virtual inline void minus_value_fast() {}
  virtual inline void enable() { plus_value(); }
  inline cr<Str> title_id() const { return _title_id; }
  inline cr<utf32> title() const { return _title; }
  inline cr<utf32> desc() const { return _desc; }
}; // Param

using Params = Vector< Shared<Param> >;

class Param_double final: public Param {
  double& _value;
  double _max {+999'999};
  double _min {-999'999};
  double _step {0.01};
  double _fast_step {0.1};

public:
  explicit Param_double(cr<Str> title_id, cr<utf32> title, cr<utf32> desc, double& value,
    double min, double max, double step, double fast_step);
  void set_value(cr<Str> val) final;
  void set_max(cr<Str> val) final;
  void set_min(cr<Str> val) final;
  void set_step(cr<Str> step) final;
  void set_fast_step(cr<Str> step) final;
  Str get_value() const final;
  Str get_max() const final;
  Str get_min() const final;
  Str get_step() const final;
  Str get_fast_step() const final;
  void plus_value() final;
  void minus_value() final;
  void plus_value_fast() final;
  void minus_value_fast() final;
}; // Param_double

class Param_int final: public Param {
  int& _value;
  int _max {+999'999};
  int _min {-999'999};
  int _step {1};
  int _fast_step {4};

public:
  explicit Param_int(cr<Str> title_id, cr<utf32> title, cr<utf32> desc, int& value,
    int min, int max, int step, int fast_step);
  void set_value(cr<Str> val) final;
  void set_max(cr<Str> val) final;
  void set_min(cr<Str> val) final;
  void set_step(cr<Str> step) final;
  void set_fast_step(cr<Str> step) final;
  Str get_value() const final;
  Str get_max() const final;
  Str get_min() const final;
  Str get_step() const final;
  Str get_fast_step() const final;
  void plus_value() final;
  void minus_value() final;
  void plus_value_fast() final;
  void minus_value_fast() final;
}; // Param_int

class Param_bool final: public Param {
  bool& _value;

public:
  explicit Param_bool(cr<Str> title_id, cr<utf32> title, cr<utf32> desc, bool& value);
  void set_value(cr<Str> val) final;
  Str get_value() const final;
  Str get_max() const final;
  Str get_min() const final;
  void plus_value() final;
  void minus_value() final;
  void plus_value_fast() final;
  void minus_value_fast() final;
  void enable() final;
}; // Param_bool

} // epge ns
