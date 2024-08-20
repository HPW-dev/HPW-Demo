#pragma once
#include "item.hpp"

// пункт меню с числовыми опциями
class Menu_item_table_row final: public Menu_item {
public:
  using Action = std::function<void ()>;
  using Content_getter = std::function<utf32 ()>;
  using Content_getters = Vector<Content_getter>;

  explicit Menu_item_table_row(cr<Action> action,
    cr<Content_getters> content_getters);
  void enable() override;
  cr<Content_getters> get_content_getters() const;

private:
  Action m_action {}; // действие при выборе строки таблицы
  Content_getters m_content_getters {}; // столбцы в строке таблицы
};
