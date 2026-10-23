#pragma once
#include "util/mem-types.hpp"
#include "util/str.hpp"

class Menu_list_item;

// ставит случайную палитру
void randomize_palette();

// пункт меню для выбора палитр
Shared<Menu_list_item> palette_list_item();

void set_palette(cr<Str> path);
