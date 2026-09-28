#pragma once
#include <thirdparty/include/nlohmann/json_fwd.hpp>
#include <optional>
#include "util/unicode.hpp"
#include "game/util/resource.hpp"

// менеджер локализованных строк
class Locale final: public Resource {
public:
  utf32 str {};
  
  inline Locale(cr<utf32> in): str{in} {}
  Locale() = default;
  ~Locale() = default;
};

void load_locales_to_store(cr<nlohmann::json> file);
// безопасное получение локализованной строки
cr<utf32> get_locale_str(cr<Str> key);
// если локализованной строки нет, то не будет результата
std::optional<utf32> get_locale_str_with_check(cr<Str> key);
void load_locale(cr<Str> user_path={});
