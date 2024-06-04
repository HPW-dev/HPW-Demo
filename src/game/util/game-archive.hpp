#pragma once
#include "util/file/archive.hpp"
#include "util/mem-types.hpp"

namespace hpw {
inline Unique<Archive> archive {}; // архив с данными игры в памяти
}

void init_archive();
