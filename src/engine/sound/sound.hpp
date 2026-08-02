#pragma once
#include "util/str.hpp"

// связывает имена треков и пути к трекам из архива
struct Sound final: public Resource  {
  /* Пути к звукам из архива.
  Несколько звуков можно прикреплять к одному имени */ 
  Strs paths {}; 
};
