#include <cassert>
#include "int-item.hpp"
#include "game/util/game-util.hpp"
#include "game/util/locale.hpp"
#include "util/str-util.hpp"

Menu_int_item::Menu_int_item(cr<utf32> new_name, Get_value _get_value,
Set_value _set_value, int _speed_step, cr<utf32> desc)
: name(new_name)
, m_desc{desc}
, get_value_f(_get_value)
, set_value_f(_set_value)
, speed_step(_speed_step)
{
 assert(get_value_f);
 assert(set_value_f);
 assert(speed_step > 0);
}

void Menu_int_item::enable() {
  plus();
}

void Menu_int_item::plus() {
  auto val = get_value_f();
  val += speed_step;
  set_value_f(val);
}
void Menu_int_item::minus() {
  auto val = get_value_f();
  val -= speed_step;
  set_value_f(val);
}

utf32 Menu_int_item::to_text() const {
  return name + U" : " + sconv<utf32>( n2s(get_value_f()) );
}

utf32 Menu_int_item::get_description() const { return m_desc; }
