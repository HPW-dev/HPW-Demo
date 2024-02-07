#pragma once
#include "util/mem-types.hpp"

class Archive;

namespace hpw {
  inline Shared<Archive> archive {}; /// архив с данными игры в памяти
}

void init_archive();
