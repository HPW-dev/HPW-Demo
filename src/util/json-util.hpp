#pragma once
#include <thirdparty/include/nlohmann/json_fwd.hpp>
#include "vector-types.hpp"
#include "macro.hpp"
#include "str.hpp"
#include "unicode.hpp"

struct Kv_utf32 {
  Str key {};
  utf32 str {};
};
using String_table_utf32 = Vector<Kv_utf32>;

// Создаёт JSON-тег с пустым содержимым, на выходе объект этого тега
nlohmann::json& make_node(nlohmann::json& dst, cr<Str> tag_name);

// получить список тегов относительно ветки src
Strs root_tags(cr<nlohmann::json> src);

// вернёт список ключей и значений: {"tag1.tag2.tag3", U"value"}
String_table_utf32 get_string_table_utf32(cr<nlohmann::json> src);

void save(cr<nlohmann::json> src, cr<Str> path, bool readable=false);
void load(nlohmann::json& dst, cr<Str> path, bool make_if_not_exist=false);

bool check(cr<nlohmann::json> node);
