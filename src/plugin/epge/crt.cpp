#include "crt.hpp"

struct Epge_crt::Impl {
  inline Impl() = default;
};

Epge_crt::Epge_crt(): impl{new_unique<Impl>()} {}
Epge_crt::~Epge_crt() {}
