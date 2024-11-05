#include "score-table.hpp"

namespace hpw {

static Score_in _record = 0;
static Score_in _scale = 100;

void add_score(Score_in val) { _record += val; }
void clear_score() { _record = 0; }
void clear_score_scale() { _scale = 0; }
Score_in get_score_internal() { return _record; }
Score_out get_score_normalized() { return _record / 100; }
Score_in get_score_scale() { return _scale; }
void set_score_scale(const Score_in scale) { _scale = scale; }

} // hpw ns
