#pragma once
#include "neu.hpp"
#include "util/mem-types.hpp"

namespace neu {

// настройки для neu::Simple
struct Simple_config {

};

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
