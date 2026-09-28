
#pragma once
#include <thirdparty/include/nlohmann/json_fwd.hpp>
#include "util/action.hpp"
#include "util/mem-types.hpp"
#include "util/macro.hpp"

class Menu;

// создаёт текстовую менюшки из json-конфига
Unique<Menu> menu_from_json(cr<Yaml> config, cr<Action_table> actions);
