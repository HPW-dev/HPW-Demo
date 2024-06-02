#pragma once
#include <cstdint>
#include "util/math/num-types.hpp"

namespace hpw {

void add_score(int64_t val);
int64_t get_score();
void clear_score();
// получить множитель очков
real get_score_scale();
// задать множитель очков
void set_score_scale(const real scale);

}
