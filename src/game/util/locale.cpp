#include "locale.hpp"
#include "store.hpp"
#include "util/str-util.hpp"
#include "util/file/yaml.hpp"
#include "util/file/file.hpp"
#include "game/core/locales.hpp"
#include "game/core/locales.hpp"
#include "game/util/locale.hpp"
#include "game/util/config.hpp"
#include "game/util/resource-helper.hpp"

cr<utf32> get_locale_str(cr<Str> key) {
  if (hpw::store_locale) {
    if (auto ret = hpw::store_locale->find(key); ret) {
      return ret->str;
    } elif (!hpw::ignore_locale_errors) {
      log_error << "not found string: \"" + key + "\"";
    }
  } elif (!hpw::ignore_locale_errors) {
    log_error << "store_locale is not initialized\n";
  }

  if ((!hpw::ignore_locale_errors))
    log_error << "not finded string \"" + key + "\"";

  static utf32 last_error;
  last_error = U"_ERR_(" + sconv<utf32>(key) + U")";
  return last_error;
}

std::optional<utf32> get_locale_str_with_check(cr<Str> key) {
  return_if (!hpw::store_locale, {});
  
  if (auto ret = hpw::store_locale->find(key); ret)
    return ret->str;
  
  log_debug << "not found string: \"" + key + "\"";
  return {};
}

void load_locale(cr<Str> user_path) {
  log_info << "загрузка локализации...";

  File mem; 
  cauto path = (user_path.empty() && hpw::config)
    ? (*hpw::config)["path"].get_str("locale", hpw::fallback_locale_path)
    : user_path;

  try {
    mem = load_res(path);
  } catch (...) {
    log_error << "ошибка при загрузке перевода \"" + path + "\". Попытка загрузить перевод \""
      + hpw::fallback_locale_path + "\"";
    mem = load_res(hpw::fallback_locale_path);
  }

  hpw::locale_path = path;
  auto yml = Yaml(mem);
  load_locales_to_store(yml);
}

void load_locales_to_store(Yaml file) {
  init_shared(hpw::store_locale);

  auto key_and_utf32s = file.get_kvu32_table();
  for (rauto [str_name, val]: key_and_utf32s) {
    auto locale = new_shared<Locale>(val);
    hpw::store_locale->push(str_name, locale);
  }
}
