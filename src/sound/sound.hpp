#pragma once
#include "util/resource.hpp"

// связывает имена треков и пути к трекам из архива
struct Sound final: public Resource  {
  Str path {}; // путь к звуку из архива
};
