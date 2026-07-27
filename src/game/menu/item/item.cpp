#include "item.hpp"

void Menu_item::enable() {}
void Menu_item::plus() {}
void Menu_item::plus_fast() { plus(); }
void Menu_item::minus() {}
void Menu_item::minus_fast() { minus(); }
utf32 Menu_item::to_text() const { return {}; }
utf32 Menu_item::get_description() const { return {}; }
