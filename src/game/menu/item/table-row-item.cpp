#include "table-row-item.hpp"

Menu_item_table_row::Menu_item_table_row(CN<Action> action,
CN<Content_getters> content_getters)
: m_action {action}
, m_content_getters {content_getters}
{}

void Menu_item_table_row::enable() {
  if (m_action)
    m_action();
}

CN<Menu_item_table_row::Content_getters>
Menu_item_table_row::get_content_getters() const {
  return m_content_getters;
}
