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
  {"NONE", []{ return new_unique<Hud_none>(); }},
  {"BRAILLE", []{ error("need impl for Braille HUD"); return Unique<Hud>{}; }}, // TODO
  {"HEX", []{ return new_unique<Hud_hex>(); }},
  {"ROMAN II", []{ error("need impl for Roman II HUD"); return Unique<Hud>{}; }}, // TODO
  {"ROMAN I", []{ return new_unique<Hud_roman>(); }},
  {"MINIMAL", []{ error("need impl for Minimal HUD"); return Unique<Hud>{}; /*return new_unique<Hud_minimal>();*/ }}, // TODO
  {"ASCI", []{ return new_unique<Hud_asci>(); }},
};

} // empty ns

Unique<Hud> make_hud(cr<Str> name) {
  try {
    return ::g_huds.at(str_toupper(name)) ();
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
  std::sort(names.begin(), names.end());
  return names;
}
