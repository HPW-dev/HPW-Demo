#include "text-item.hpp"

Menu_text_item::Menu_text_item(cr<utf32> _name, cr<Action> _action, cr<Getter> _get_value, cr<utf32> desc)
: action {_action}
, name {_name}
, m_desc {desc}
, get_value {_get_value}
{}

void Menu_text_item::enable() {
  if (action)
    action();
}

utf32 Menu_text_item::to_text() const {
  utf32 ret {name};
  if (get_value) {
    cauto value = get_value();
    if ( !value.empty())
      ret += U" -- " + value;
  }
  return ret;
}

utf32 Menu_text_item::get_description() const { return m_desc; }
