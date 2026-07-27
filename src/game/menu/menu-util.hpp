#pragma once
#include "util/mem-types.hpp"

class Menu_item;

// создаёт разделить в интерфейсе меню. Если задан flag, то он станет true при нажатии на разделитель
Shared<Menu_item> make_menu_separator(bool* flag);
