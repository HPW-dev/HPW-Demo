#include <cassert>
#include "neu-simple.hpp"
#include "util/file/yaml.hpp"

namespace neu {

struct Simple::Impl {
  Simple& _master;
  Simple_config _config {};

  inline explicit Impl(Simple& master, cr<Simple_config> config)
  : _master {master}, _config {config} {
    check_config();
    init();
  }

  inline void save(Yaml& dst) {
    // TODO
  }

  inline void load(cr<Yaml> src) {
    // TODO
  }

  inline Base& operator =(cr<Base> other) {
    // TODO
    return _master;
  }

  inline void update() {
    // TODO
  }

  inline void check_config() {
    assert(!_config.inputs.neurons.empty());
    assert(!_config.outputs.neurons.empty());
    assert(!_config.hiden_layers.empty());

    for (crauto x: _config.inputs.neurons) {
      assert(!x.name.empty());
      assert(x.setter);
    }

    for (crauto x: _config.hiden_layers)
      assert(x.hiden_neurons > 0);
    
    for (crauto x: _config.outputs.neurons) {
      assert(!x.name.empty());
      assert(x.getter);
    }
  }

  inline void init() {
    uint weight_count {};
    weight_count += _config.inputs.neurons.size() * _config.hiden_layers.at(0).hiden_neurons;
    weight_count += _config.outputs.neurons.size() * _config.hiden_layers.back().hiden_neurons;

    cauto layers = _config.hiden_layers.size();
    if (layers) {
      cfor (i, layers-1) {
        weight_count +=
          _config.hiden_layers.at(i).hiden_neurons *
          _config.hiden_layers.at(i+1).hiden_neurons;
      }
    } 

    assert(weight_count > 0);
    _master.weights().resize(weight_count);
  }
}; // Impl

Simple::Simple(cr<Simple_config> config): _impl{new_unique<Impl>(*this, config)} {}
Simple::~Simple() {}
void Simple::save(Yaml& dst) { _impl->save(dst); }
void Simple::load(cr<Yaml> src) { _impl->load(src); }
Base& Simple::operator =(cr<Base> other) { return _impl->operator =(other); }
void Simple::update() { _impl->update(); }

} // neu ns
