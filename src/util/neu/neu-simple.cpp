#include "neu-simple.hpp"
#include "util/file/yaml.hpp"

namespace neu {

struct Simple::Impl {
  Simple& _master;

  inline Impl(Simple& master): _master {master} {}

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
}; // Impl

Simple::Simple(): _impl{new_unique<Impl>(*this)} {}
Simple::~Simple() {}
void Simple::save(Yaml& dst) { _impl->save(dst); }
void Simple::load(cr<Yaml> src) { _impl->load(src); }
Base& Simple::operator =(cr<Base> other) { return _impl->operator =(other); }
void Simple::update() { _impl->update(); }

} // neu ns
