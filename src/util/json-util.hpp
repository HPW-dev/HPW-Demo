#pragma once
#include <thirdparty/include/nlohmann/json_fwd.hpp>
#include "macro.hpp"
#include "str.hpp"

// Создаёт JSON-тег с пустым содержимым, на выходе объект этого тега
nlohmann::json& make_node(nlohmann::json& dst, cr<Str> tag_name);

// получить список тегов относительно ветки src
Strs root_tags(cr<nlohmann::json> src);
