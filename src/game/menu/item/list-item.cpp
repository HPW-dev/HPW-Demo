#include "list-item.hpp"
#include "util/str-util.hpp"
#include "util/log.hpp"
#include "game/util/locale.hpp"

Menu_list_item::Menu_list_item(cr<utf32> title, cr<Menu_list_item::Items> items,
cr<Default_select_getter> default_select_getter)
: m_items {items}
, m_title {title}
, _default_select_getter {default_select_getter}
{}

void Menu_list_item::enable() { plus(); }

void Menu_list_item::plus() {
  return_if(m_items.empty());

  m_selected = (m_selected + 1) % m_items.size();
  m_items.at(m_selected).action();
}

void Menu_list_item::minus() {
  return_if(m_items.empty());

  if (m_selected == 0)
    m_selected = m_items.size()-1;
  else
    --m_selected;
  m_items.at(m_selected).action();
}

utf32 Menu_list_item::to_text() const {
  return m_title + U" : " + ( m_items.empty()
    ? get_locale_str("common.empty")
    : sconv<utf32>(m_items.at(m_selected).name) );
}

utf32 Menu_list_item::get_description() const { return m_items.at(m_selected).desc; }

void Menu_list_item::update(const Delta_time dt) {
  return_if(m_items.empty());

  if (_default_select_getter) {
    cauto overrided_idx = _default_select_getter();
    if (overrided_idx < m_items.size()) {
      m_selected = overrided_idx;
    } else {
      log_warning << "WARNING: неверный id переопределённого элемента (" + n2s(overrided_idx) + ")";
      m_selected = overrided_idx % m_items.size();
    }

    // больше выбор не требуется
    _default_select_getter = {};
  }
}
