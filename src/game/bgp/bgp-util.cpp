#include "bgp-util.hpp"
#include "bgp.hpp"
#include "util/rnd-table.hpp"

namespace bgp {

Str random_name() { return Rnd_table<Str>(all_names()).rnd_fast(); }

} // bgp ns
