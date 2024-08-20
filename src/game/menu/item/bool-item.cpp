#include <cassert>
#include "bool-item.hpp"
#include "game/util/game-util.hpp"
#include "game/util/locale.hpp"

Menu_bool_item::Menu_bool_item(cr<utf32> new_name, Get_value _get_value,
  Set_value _set_value, cr<utf32> desc)
: name(new_name)
, m_desc(desc)
, get_value_f(_get_value)
, set_value_f(_set_value)
{
 assert(get_value_f);
 assert(set_value_f);
}

void Menu_bool_item::enable() {
  bool val = get_value_f();
  val = !val;
  set_value_f(val);
}

void Menu_bool_item::plus() { enable(); }
void Menu_bool_item::minus() { enable(); }

utf32 Menu_bool_item::to_text() const {
  utf32 yn_str = get_value_f()
    ? get_locale_str("common.on")
    : get_locale_str("common.off");
  return name + U" : " + yn_str;
}

utf32 Menu_bool_item::get_description() const { return m_desc; }
