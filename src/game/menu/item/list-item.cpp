#include <cassert>
#include "list-item.hpp"
#include "util/str-util.hpp"

Menu_list_item::Menu_list_item(CN<utf32> title, CN<Menu_list_item::Items> items)
: m_items {items}
, m_title {title}
{ assert( !m_items.empty()); }

void Menu_list_item::enable() { plus(); }

void Menu_list_item::plus() {
  m_selected = (m_selected + 1) % m_items.size();
  m_items.at(m_selected).action();
}
void Menu_list_item::minus() {
  if (m_selected == 0)
    m_selected = m_items.size()-1;
  else
    --m_selected;
  m_items.at(m_selected).action();
}

utf32 Menu_list_item::to_text() const
  { return m_title + U" : " + sconv<utf32>( m_items.at(m_selected).name ); }

utf32 Menu_list_item::get_description() const { return m_items.at(m_selected).desc; }
