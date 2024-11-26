#include <functional>
#include <unordered_map>
#include "hud-util.hpp"
#include "hud-asci.hpp"
#include "hud-none.hpp"
#include "hud-hex.hpp"
#include "hud-roman.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"

namespace {

using Hud_maker = std::function< Unique<Hud> () >;

sconst std::unordered_map<Str, Hud_maker> g_huds {
  {"asci", []{ return new_unique<Hud_asci>(); }},
  {"none", []{ return new_unique<Hud_none>(); }},
  {"minimal", []{ error("need impl for minimal HUD"); return Unique<Hud>{}; /*return new_unique<Hud_minimal>();*/ }},
  {"hex", []{ return new_unique<Hud_hex>(); }},
  {"roman", []{ return new_unique<Hud_roman>(); }},
};

} // empty ns

Unique<Hud> make_hud(cr<Str> name) {
  try {
    return ::g_huds.at(str_tolower(name)) ();
  } catch (...) {
    hpw_log("не удалось создать интерфейс с именем \"" + name +
      "\". Будет создан Hud_asci по умолчанию\n", Log_stream::warning);
  }

  return new_unique<Hud_asci>();
}

Strs hud_names() {
  Strs names;
  for (crauto [name, _]: ::g_huds)
    names.push_back(name);
  return names;
}
