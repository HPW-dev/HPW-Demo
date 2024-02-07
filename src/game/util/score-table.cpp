#include "score-table.hpp"

namespace hpw {

static int64_t m_record = 0;

void add_score(int64_t val) { m_record += val; }
void clear_score() { m_record = 0; }
int64_t get_score() { return m_record; }

} // hpw ns
