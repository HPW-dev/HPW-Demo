#pragma once
#include "util/mem-types.hpp"

class Menu_list_item;

void randomize_palette(); // ставит случайную палитру
Shared<Menu_list_item> get_palette_list(); // пункт меню для выбора палитр
