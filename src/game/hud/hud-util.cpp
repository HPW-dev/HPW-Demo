#include <functional>
#include <unordered_map>
#include "hud-util.hpp"
#include "hud-asci.hpp"
#include "hud-none.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"

namespace {

using Hud_maker = std::function< Unique<Hud> () >;

sconst std::unordered_map<Str, Hud_maker> g_huds {
  {"asci", []{ return new_unique<Hud_asci>(); }},
  {"none", []{ return new_unique<Hud_none>(); }},
};

} // empty ns

Unique<Hud> make_hud(cr<Str> name) {
  try {
    return ::g_huds.at(str_tolower(name)) ();
  } catch (...) {
    hpw_log("не удалось создать интерфейс с именем \"" << name << "\". Будет создан Hud_asci по умолчанию\n");
  }

  return new_unique<Hud_asci>();
}

Strs hud_names() {
  Strs names;
  for (crauto [name, _]: ::g_huds)
    names.push_back(name);
  return names;
}
