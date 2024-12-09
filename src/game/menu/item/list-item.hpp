#pragma once
#include "item.hpp"

// именованные параметры вызывающие действие
class Menu_list_item final: public Menu_item {
public:
  using Action = std::function<void ()>;
  using Default_select_getter = std::function<std::size_t ()>;
  struct Item {
    utf32 name {};
    utf32 desc {};
    Action action {};
  };
  using Items = Vector<Item>;

private:
  Items m_items {};
  utf32 m_title {};
  std::size_t m_selected {};
  Default_select_getter _default_select_getter {};

public:
  /** @param title название списка
  * @param items элементы списка
  @param default_select_getter через что узнать какой сейчас выбран элемент по умолчанию */
  explicit Menu_list_item(cr<utf32> title, cr<Items> items,
    cr<Default_select_getter> default_select_getter = {});
  void enable() override;
  utf32 to_text() const override;
  utf32 get_description() const override;
  void plus() override;
  void minus() override;
  void update(const Delta_time dt) override;
};
