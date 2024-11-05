#pragma once
#include <cstdint>
#include "util/math/num-types.hpp"

namespace hpw {

using Score_in = int64_t; // int с имитацией float через умножение на 100
using Score_out = int64_t; // вариант для отображения на экране и сохранении в реплее (делён на 100)

// можно как добавить очки, так и отнять
void add_score(Score_in val);
// узнать сколько сейчас очков у игрока (во внутреннем представлении)
Score_in get_score_internal();
// узнать сколько сейчас очков у игрока (вариант для отображения на экране и сохранении в реплее)
Score_out get_score_normalized();
// сборс рекорда игрока
void clear_score();
// сборс множителя очков
void clear_score_scale();
// получить множитель очков
Score_in get_score_scale();
// задать множитель очков
void set_score_scale(const Score_in scale);

}
