#include "score-table.hpp"

namespace hpw {

static int64_t m_record = 0;
static real m_scale = 1;

void add_score(int64_t val) { m_record += val; }
void clear_score() { m_record = 0; }
int64_t get_score() { return m_record; }
real get_score_scale() { return m_scale; }
void set_score_scale(const real scale) { m_scale = scale; }

} // hpw ns
