#include <ranges>
#include <functional>
#include <map>
#include "hud-util.hpp"
#include "hud-asci.hpp"
#include "hud-none.hpp"
#include "hud-hex.hpp"
#include "hud-roman.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"

namespace {

using Hud_maker = std::function<Unique<Hud> ()>;

sconst std::map<Str, Hud_maker> g_huds {
  {"ASCI", []{ return new_unique<Hud_asci>(); }},
  {"HEX", []{ return new_unique<Hud_hex>(); }},
  {"ROMAN I", []{ return new_unique<Hud_roman>(); }},
  //{"ROMAN II", []{ error("need impl for Roman II HUD"); return Unique<Hud>{}; }}, // TODO
  //{"BRAILLE", []{ error("need impl for Braille HUD"); return Unique<Hud>{}; }}, // TODO
  //{"MINIMAL", []{ error("need impl for Minimal HUD"); return Unique<Hud>{}; /*return new_unique<Hud_minimal>();*/ }}, // TODO
  {"NONE", []{ return new_unique<Hud_none>(); }},
};

} // empty ns

Unique<Hud> make_hud(cr<Str> name) {
  try {
    return ::g_huds.at(str_toupper(name)) ();
  } catch (...) {
    log_warning << "не удалось создать интерфейс с именем \"" + name +
      "\". Будет создан Hud_asci по умолчанию";
  }

  return new_unique<Hud_asci>();
}

Strs hud_names() { return ::g_huds | std::views::keys | std::ranges::to<Strs>(); }
