#pragma once
#include "item.hpp"

// именованные параметры вызывающие действие
class Menu_list_item final: public Menu_item {
public:
  using Action = std::function<void ()>;
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

public:
  explicit Menu_list_item(cr<utf32> title, cr<Items> items,
    const std::size_t default_selected = 0);
  void enable() override;
  utf32 to_text() const override;
  utf32 get_description() const override;
  void plus() override;
  void minus() override;
};
