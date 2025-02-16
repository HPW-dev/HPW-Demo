#pragma once
#include "util/vector-types.hpp"
#include "util/math/num-types.hpp"
#include "util/macro.hpp"
#include <functional>

class Yaml;

namespace neu {
  
using Weight = real;
using Weights = Vector<Weight>;
using Activator = std::function<Weight (Weight)>;

class Base {
public:
  Base() = default;
  virtual ~Base() = default;

  virtual void save(Yaml& dst) = 0;
  virtual void load(cr<Yaml> src) = 0;
  virtual Base& operator =(cr<Base> other) = 0;
  virtual void update() = 0;

  inline Weights& weights() { return _hiden_weights; };
  inline cr<Weights> weights() const { return _hiden_weights; };

private:
  Weights _hiden_weights {}; // все веса скрытых слоёв нейросети
}; // Base

} // neu ns
