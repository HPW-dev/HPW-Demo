#pragma once
#include "neu.hpp"

namespace neu {

// задаёт весам нейросети случайные значения
void randomize(Base& dst, Weight wmin=-1, Weight wmax=1, bool allow_zero=true);

} // neu ns
