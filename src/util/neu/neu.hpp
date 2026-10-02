#pragma once
#include <functional>
#include <thirdparty/include/nlohmann/json_fwd.hpp>
#include "util/vector-types.hpp"
#include "util/num-types.hpp"
#include "util/macro.hpp"

namespace neu {
  
using Weight = real;
using Weights = Vector<Weight>;
using Activator = std::function<Weight (Weight)>;

class Base {
public:
  Base() = default;
  virtual ~Base() = default;

  virtual void save(nlohmann::json& dst) = 0;
  virtual void load(cr<nlohmann::json> src) = 0;
  virtual Base& operator =(cr<Base> other) = 0;
  virtual void update() = 0;

  inline Weights& weights() { return _hiden_weights; };
  inline cr<Weights> weights() const { return _hiden_weights; };

private:
  Weights _hiden_weights {}; // все веса скрытых слоёв нейросети
}; // Base

} // neu ns
