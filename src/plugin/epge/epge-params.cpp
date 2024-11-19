#include <cassert>
#include <algorithm>
#include "epge-params.hpp"
#include "util/str-util.hpp"

namespace epge {

Param::Param(cr<Str> title, cr<Str> desc)
: _title {title}
, _desc {desc}
{
  assert(!_title.empty());
}

Param_int::Param_int(cr<Str> title, cr<Str> desc, int& value, int min, int max, int step, int fast_step)
: Param {title, desc}
, _value {value}
, _max {max}
, _min {min}
, _step {step}
, _fast_step {fast_step}
{
  assert(_max > _min);
  assert(_step > 0);
  assert(_fast_step > 0);
}

void Param_int::set_value(cr<Str> val) { _value = s2n<int>(val); }
void Param_int::set_max(cr<Str> val) { _max = s2n<int>(val); assert(_max > _min); }
void Param_int::set_min(cr<Str> val) { _min = s2n<int>(val); assert(_max > _min); }
void Param_int::set_step(cr<Str> step) { _step = s2n<int>(step); assert(_step > 0); }
void Param_int::set_fast_step(cr<Str> step) { _fast_step = s2n<int>(step); assert(_fast_step > 0); }
Str Param_int::get_value() const { return n2s(_value); }
Str Param_int::get_max() const { return n2s(_max); }
Str Param_int::get_min() const { return n2s(_min); }
Str Param_int::get_step() const { return n2s(_step); }
Str Param_int::get_fast_step() const { return n2s(_fast_step); }
void Param_int::plus_value() { _value = std::clamp(_value + _step, _min, _max); }
void Param_int::minus_value() { _value = std::clamp(_value - _step, _min, _max); }
void Param_int::plus_value_fast() { _value = std::clamp(_value + _fast_step, _min, _max); }
void Param_int::minus_value_fast() { _value = std::clamp(_value - _fast_step, _min, _max); }

Param_double::Param_double(cr<Str> title, cr<Str> desc, double& value, double min, double max, 
double step, double fast_step)
: Param {title, desc}
, _value {value}
, _max {max}
, _min {min}
, _step {step}
, _fast_step {fast_step}
{
  assert(_max > _min);
  assert(_step > 0);
  assert(_fast_step > 0);
}

void Param_double::set_value(cr<Str> val) { _value = s2n<double>(val); }
void Param_double::set_max(cr<Str> val) { _max = s2n<double>(val); assert(_max > _min); }
void Param_double::set_min(cr<Str> val) { _min = s2n<double>(val); assert(_max > _min); }
void Param_double::set_step(cr<Str> step) { _step = s2n<double>(step); assert(_step > 0); }
void Param_double::set_fast_step(cr<Str> step) { _fast_step = s2n<double>(step); assert(_fast_step > 0); }
Str Param_double::get_value() const { return n2s(_value); }
Str Param_double::get_max() const { return n2s(_max); }
Str Param_double::get_min() const { return n2s(_min); }
Str Param_double::get_step() const { return n2s(_step); }
Str Param_double::get_fast_step() const { return n2s(_fast_step); }
void Param_double::plus_value() { _value = std::clamp(_value + _step, _min, _max); }
void Param_double::minus_value() { _value = std::clamp(_value - _step, _min, _max); }
void Param_double::plus_value_fast() { _value = std::clamp(_value + _fast_step, _min, _max); }
void Param_double::minus_value_fast() { _value = std::clamp(_value - _fast_step, _min, _max); }

inline static bool s2bool(cr<Str> str) { 
  cauto str_lower = str_tolower(str);
  return_if (str_lower == "true", true);
  return_if (str_lower == "+", true);
  return_if (str_lower == "yes", true);
  return_if (str_lower == "1", true);
  return_if (str_lower == "on", true);
  return_if (str_lower == "enable", true);
  return_if (str_lower == "enabled", true);
  return_if (str_lower == "ok", true);
  return false;
}

inline static Str bool2s(const bool val) {
  return val ? "true" : "false";
}

Param_bool::Param_bool(cr<Str> title, cr<Str> desc, bool& value)
: Param {title, desc}
, _value {value}
{}

void Param_bool::set_value(cr<Str> val) { _value = s2bool(val); }
Str Param_bool::get_value() const { return bool2s(_value); }
Str Param_bool::get_max() const { return "true"; }
Str Param_bool::get_min() const { return "false"; }
void Param_bool::plus_value() { _value = true; }
void Param_bool::minus_value() { _value = false; }
void Param_bool::plus_value_fast() { _value = true; }
void Param_bool::minus_value_fast() { _value = false; }

} // epge ns 
