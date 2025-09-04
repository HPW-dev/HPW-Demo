
#pragma once
#include "util/action.hpp"
#include "util/mem-types.hpp"
#include "util/macro.hpp"

class Yaml;
class Menu;

// создаёт текстовую менюшки из yaml-конфига
Unique<Menu> menu_from_yaml(cr<Yaml> config, cr<Action_table> actions);
