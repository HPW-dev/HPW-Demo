#include <cassert>
#include "double-item.hpp"
#include "game/util/game-util.hpp"
#include "game/util/locale.hpp"
#include "util/str-util.hpp"

Menu_double_item::Menu_double_item(cr<utf32> new_name, Get_value _get_value,
Set_value _set_value, const Delta_time _speed_step, cr<utf32> desc)
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

void Menu_double_item::enable() {
  plus();
}

void Menu_double_item::plus() {
  auto val = get_value_f();
  val += speed_step;
  set_value_f(val);
}
void Menu_double_item::minus() {
  auto val = get_value_f();
  val -= speed_step;
  set_value_f(val);
}

utf32 Menu_double_item::to_text() const {
  return name + U" : " + sconv<utf32>( n2s(get_value_f(), 3) );
}

utf32 Menu_double_item::get_description() const { return m_desc; }
