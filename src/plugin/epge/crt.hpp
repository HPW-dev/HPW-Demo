#pragma once
#include "epge.hpp"
#include "util/mem-types.hpp"

// эффект пиксельного дисплея 
class Epge_crt: public epge::Base {
  struct Impl;
  Unique<Impl> impl {};

public:
  Epge_crt();
  ~Epge_crt();
};
