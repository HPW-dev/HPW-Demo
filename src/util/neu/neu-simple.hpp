#pragma once
#include "neu.hpp"
#include "util/mem-types.hpp"

namespace neu {

// простая нейросеть из полносвязных перцептронов
class Simple: public Base {
public:
  Simple();
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
