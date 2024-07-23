#include "locale.hpp"
#include "util/str-util.hpp"
#include "util/file/yaml.hpp"
#include "game/core/locales.hpp"
#include "store.hpp"

void load_locales_to_store(Yaml file) {
  if (!hpw::store_locale)
    init_shared(hpw::store_locale);

  auto key_and_utf32s = file.get_kvu32_table();
  for (nauto [str_name, val]: key_and_utf32s) {
    auto locale = new_shared<Locale>(val);
    hpw::store_locale->push(str_name, locale);
  }
}
