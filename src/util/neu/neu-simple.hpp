#pragma once
#include "neu.hpp"
#include "util/mem-types.hpp"
#include "util/str.hpp"

namespace neu {

// настройки для neu::Simple
struct Simple_config {
  using Getter = std::function<Weight ()>;
  using Setter = std::function<void (const Weight)>;

  struct Input_neuron {
    Str name {};
    Setter setter {};
  };

  struct Input_neurons {
    Vector<Input_neuron> neurons {};
    Activator activator {};
  };

  struct Output_neuron {
    Str name {};
    Getter getter {};
  };

  struct Output_neurons {
    Vector<Output_neuron> neurons {};
    Activator activator {};
  };

  struct Hiden_layer {
    uint hiden_neurons {}; // сколько нейронов на скрытом слое
    Weight bias {};
    Activator activator {};
  };

  Input_neurons inputs {};
  Output_neurons outputs {};
  Vector<Hiden_layer> hiden_layers {};
}; // Simple_config

// простая нейросеть из полносвязных перцептронов
class Simple: public Base {
public:
  explicit Simple(cr<Simple_config> config);
  ~Simple();

  void save(Yaml& dst) override;
  void load(cr<Yaml> src) override;
  Base& operator =(cr<Base> other) override;
  void update() override;

private:
  struct Impl;
  Unique<Impl> _impl {};
};

} // neu ns
